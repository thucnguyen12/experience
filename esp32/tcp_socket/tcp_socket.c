#include "tcp_socket.h"
#include "esp_log.h"
#include <sys/param.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
// #include "app_cli.h"
#include "freertos/timers.h"
#include "lwrb.h"
#include "app_ble.h"

#define PORT                        80
#define KEEPALIVE_IDLE              60
#define KEEPALIVE_INTERVAL          120
#define KEEPALIVE_COUNT             3
#define TCP_RX_BUFFER_SIZE          256
#define PORT 80
#define TCP_CONSOLE_TIMEOUT         2000
#define MAX_SIZE_DATA_SOCKET        256

#define SHOW_ENTER_USERNAME()       tcp_server_send((uint8_t*)"Enter username: ", strlen("Enter username: "))
typedef enum
{
    LOGIN_STATE_WAIT_FOR_USERNAME,
    LOGIN_STATE_WAIT_FOR_PASSWORD,
    LOGIN_STATE_LOGINNED
} login_state_t;


static const char *TAG  = "tcp_console";

static bool m_task_created = false;
// static bool m_cli_started = false;
static SemaphoreHandle_t m_sem_lock = NULL;
static void tcp_server_send(uint8_t *buffer, uint32_t size);
int32_t tcp_console_puts(char *msg);
static char *m_tcp_rx_buffer;
static int sock = -1;
static bool m_client_connected = false;
static const char *m_default_console_password = "";
static const char *m_default_console_username = "";
static char m_password_buffer[64];
static char m_username_buffer[64];
static uint32_t m_user_type_index = 0;
static uint32_t m_wrong_password_count = 0;
static TimerHandle_t m_timer_close_socket = NULL;
static void on_timeout_reset_wrong_pass_count(void *timer);
static login_state_t m_login_state = LOGIN_STATE_WAIT_FOR_USERNAME;

lwrb_t data_from_socket_ringbuffer;
uint8_t socket_data_buffer [MAX_SIZE_DATA_SOCKET];

char ip_add_string_send_to_mobile [128];

void tcp_console_set_password(char *password)
{
    m_default_console_password = password;
}

void tcp_console_set_username(char *username)
{
    m_default_console_username = username;
}

// static app_cli_cb_t m_tcp_cli = 
// {
//     .puts = tcp_server_send,
//     .printf = tcp_console_puts,
//     .terminate = tcp_console_close
// };


// static void show_hello_msg(void)
// {
//     const char VS[] = "\r\n\t\tVSpeaker shell\r\n";
//     const char Welcome[] = "\r\t\tCopyright by xTeam@bytech.vn";
//     const char Bytech[] = 
//     {
//         "\r\n    ______  ______________________  __       _______ ______\r\n"
//         "   / __ ) \\/ /_  __/ ____/ ____/ / / /      / / ___// ____/\r\n"
//         "  / __  |\\  / / / / __/ / /   / /_/ /  __  / /\\__ \\/ /     \r\n"
//         " / /_/ / / / / / / /___/ /___/ __  /  / /_/ /___/ / /___\r\n"
//         "/_____/ /_/ /_/ /_____/\\____/_/ /_/   \\____//____/\\____/   \r\n\r\n"
//     };
//     tcp_console_puts((char*)VS);
//     vTaskDelay(10);
//     tcp_console_puts((char*)Welcome);
//     vTaskDelay(10);
//     tcp_console_puts((char*)Bytech);
//     vTaskDelay(10);
// }

uint8_t data_read_from_buffer [256];

void poll_tcp_data (void)
{
    uint32_t time_now = 0;
    time_now = xTaskGetTickCount();
    static uint32_t last_time_poll = 0;
    if (time_now - last_time_poll > 20)
    {
        last_time_poll = time_now;
        memset (data_read_from_buffer, 0, 256);
        uint16_t read_count = lwrb_read(&data_from_socket_ringbuffer, data_read_from_buffer, 256);
        if (read_count > 0)
        {
            ESP_LOGI (TAG, "DATA WILL PROCESS: %s with len: %d", data_read_from_buffer, read_count);

            process_ble_msg (data_read_from_buffer, read_count);
        }
    }
}


static void on_timeout_reset_wrong_pass_count(void *timer)
{
    m_wrong_password_count = 0;
}

static void poll_socket_data(const int sock)
{
    int len;
    if (!m_tcp_rx_buffer)
    {
        m_tcp_rx_buffer = malloc(TCP_RX_BUFFER_SIZE);
        assert(m_tcp_rx_buffer);
    }
    // SHOW_ENTER_USERNAME();
    do 
    {
        len = recv(sock, m_tcp_rx_buffer, TCP_RX_BUFFER_SIZE - 1, 0);
        if (len < 0) 
        {
            ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
            memset(m_password_buffer, 0, sizeof(m_password_buffer));
            m_user_type_index = 0;
            break;
        } 
        else if (len == 0) 
        {
            ESP_LOGW(TAG, "Connection closed");
            memset(m_password_buffer, 0, sizeof(m_password_buffer));
            m_user_type_index = 0;
            break;
        } 
        else 
        {
            ESP_LOGI(TAG, "Received %d bytes: %*.s", len, len, m_tcp_rx_buffer);
            lwrb_write (&data_from_socket_ringbuffer, m_tcp_rx_buffer, len);
        }
        poll_tcp_data ();
        
        vTaskDelay(10/portTICK_PERIOD_MS);
    } while (1);

    // m_login_state = LOGIN_STATE_WAIT_FOR_USERNAME;
    // m_user_type_index = 0;
    // memset(m_username_buffer, 0, sizeof(m_username_buffer));
    // memset(m_password_buffer, 0, sizeof(m_password_buffer));

    if (m_tcp_rx_buffer)
    {
        free(m_tcp_rx_buffer);
        m_tcp_rx_buffer = NULL;
    }
}

static void tcp_server_send(uint8_t *buffer, uint32_t size)
{
    if (m_sem_lock && xSemaphoreTake(m_sem_lock, TCP_CONSOLE_TIMEOUT))
    {
        if (m_client_connected)
        {
            #if 1
                    uint32_t timeout = 100;
                    int to_write = size;
                    while (to_write > 0 && timeout) 
                    {
                        int written = send(sock, buffer + (size - to_write), to_write, 0);
                        if (written < 0) 
                        {
                            ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
                            to_write = 0;
                        }
                        else
                        {
                            to_write -= written;
                            timeout--;
                            vTaskDelay(1);
                        }
                    }
            #else
                    send(sock, buffer, size, 0);  
            #endif
        }
        // for (uint32_t i = 0; i < size; i++)
        // {
        //     putc(buffer[i], stdout);
        // }
        // fflush(stdout);
        xSemaphoreGive(m_sem_lock);
    }
}

int32_t tcp_console_puts(char *msg)
{
    uint32_t len = strlen(msg);
    if (m_login_state == LOGIN_STATE_LOGINNED)
    {
        tcp_server_send((uint8_t*)msg, len);
    }
    
    if (m_sem_lock && xSemaphoreTake(m_sem_lock, TCP_CONSOLE_TIMEOUT))
    {
        printf("%s", msg);
        fflush(stdout);
        xSemaphoreGive(m_sem_lock);
    }

    return len;
}

void tcp_console_close(void)
{
    if (sock != -1)
    {
        shutdown(sock, 0);
        close(sock);
        sock = -1;
        memset(m_username_buffer, 0, sizeof(m_username_buffer));
        memset(m_password_buffer, 0, sizeof(m_password_buffer));
        m_user_type_index = 0;
        m_login_state = LOGIN_STATE_WAIT_FOR_USERNAME;
    }
}

extern bool eth_started;
extern bool wifi_started;
static void tcp_console_task(void *arg)
{
    while (!eth_started && (!wifi_started)) 
    {
        vTaskDelay(1000/portTICK_PERIOD_MS);
    }

    lwrb_init (&data_from_socket_ringbuffer, socket_data_buffer, MAX_SIZE_DATA_SOCKET);
    char addr_str[128];
    int addr_family = AF_INET;
    int ip_protocol = 0;
    int keepAlive = 1;
    int keepIdle = KEEPALIVE_IDLE;
    int keepInterval = KEEPALIVE_INTERVAL;
    int keepCount = KEEPALIVE_COUNT;
    struct sockaddr_storage dest_addr;

    if (addr_family == AF_INET) 
    {
        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;
    }

    int listen_sock = socket(addr_family, SOCK_STREAM, ip_protocol);
    if (listen_sock < 0) 
    {
        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
        return;
    }
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    ESP_LOGI(TAG, "Socket created");

    int err = bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
    if (err != 0) 
    {
        ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
        ESP_LOGE(TAG, "IPPROTO: %d", addr_family);
        goto CLEAN_UP;
    }
    ESP_LOGI(TAG, "Socket bound, port %d", PORT);

    err = listen(listen_sock, 1);
    if (err != 0) 
    {
        ESP_LOGE(TAG, "Error occurred during listen: errno %d", errno);
        goto CLEAN_UP;
    }

    while (1) 
    {
        ESP_LOGI(TAG, "Socket listening");
        struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
        socklen_t addr_len = sizeof(source_addr);
        sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
        if (sock < 0) 
        {
            ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
            break;
        }

        // Set tcp keepalive option
        setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
        setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
        // Convert ip address to string
        if (source_addr.ss_family == PF_INET) 
        {
            inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
        }
#warning "send ip to ble if conneted" 
        ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);
        memset (ip_add_string_send_to_mobile, 0, sizeof (ip_add_string_send_to_mobile));
        memcpy (ip_add_string_send_to_mobile, addr_str, sizeof (addr_str));
        if (xSemaphoreTake(m_sem_lock, portMAX_DELAY))
        {
            m_client_connected = true;
            xSemaphoreGive(m_sem_lock);
        }
        // m_login_state = LOGIN_STATE_WAIT_FOR_USERNAME;
    
        poll_socket_data(sock);
        m_client_connected = false;
        
        shutdown(sock, 0);
        close(sock);
        sock = -1;
        break;
    }

CLEAN_UP:
    close(listen_sock);
    listen_sock = -1;
    m_task_created = false;
    vTaskDelete(NULL);
}

void send_tcp_msg_to_server (uint8_t* buffer, uint32_t size)
{
    if (!eth_started && (!wifi_started)) 
    {
        ESP_LOGW (TAG, "HAVE NOT CONNECT TO ANY NETWORK YET");
        return;
        // vTaskDelay(1000/portTICK_PERIOD_MS);
    }
    tcp_server_send (buffer, size);
}

// static void uart_console_task(void *arg)
// {
//     // if (m_cli_started == false)
//     // {
//     //     m_cli_started = true;
//     //     app_cli_start(&m_tcp_cli);
//     // }   

//     char c;
//     int nread;
//     while (1)
//     {
// cont:
//         nread = fread(&c, 1, 1, stdin);
//         if(nread > 0 && c != 0xFF)
//         {
//             // app_cli_poll(c);
//             goto cont;
//         }

//         vTaskDelay(1);
//     }
// }

// bool get_status_of_tcp_socket_task(void)
// {
//     return m_task_created;
// }

//ham nay goi lien tuc trong while 1
void tcp_console_start(void)
{
    if (m_task_created)
    {
        return;
    }
    ESP_LOGD(TAG, "Start TCP console task");
    m_task_created = true;
    if (m_timer_close_socket)
    {
        xTimerStop(m_timer_close_socket, 0);
        xTimerDelete(m_timer_close_socket, 0);
        m_timer_close_socket = NULL;
    }
    if (!m_timer_close_socket)
    {
        m_timer_close_socket = xTimerCreate("tim_close_soc", 30000, pdFALSE, NULL, on_timeout_reset_wrong_pass_count);
    }
    if (!m_sem_lock)
    {
        m_sem_lock = xSemaphoreCreateMutex();
        xSemaphoreGive(m_sem_lock);
    }
    // assert(xTaskCreate(uart_console_task, "uart_console", 4096, NULL, tskIDLE_PRIORITY+2, NULL));
    // vTaskDelay(100);
    assert(xTaskCreate(tcp_console_task, "tcp_console", 4096, NULL, tskIDLE_PRIORITY+1, NULL));
}
