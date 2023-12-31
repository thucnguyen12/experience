/*
 * app_wifi.c
 *
 *  Created on: Apr 27, 2021
 *      Author: huybk
 */


#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_log.h"
#include <string.h>
#include "http_request.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"


#include "wireless_protocol.h"
//#include "test/unity/examples/unity_config.h"
//#include "unity/src/unity.h"
//#include "common.h"
#include "cJSON.h"

static const char *TAG = "http_request";

static char REQUEST[256]; //= "GET " WEB_PATH " HTTP/1.0\r\n"
    // "Host: "WEB_SERVER":"WEB_PORT"\r\n"
    // "User-Agent: esp32_smart_module\r\n"
    // "Accept:*/*\r\n"
    // "Authorization:Basic ZmlyZS1zYWZlOmFiY2QxMjM0\r\n"
    // "\r\n";



cJSON *parse_json(char *content)
{
    cJSON *parsed = NULL;
    parsed = cJSON_Parse(content);
    return parsed;
}

char recv_buf[256];
static char queuebuf [256];
extern QueueHandle_t mqtt_info_queue;
extern bool http_task_alive;
mqtt_info_struct mqtt_info;
static uint8_t http_task_count = 0;


void http_get_task(void *pvParameters)
{
    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;
    sprintf (REQUEST, "GET  /api/server-info  HTTP/1.0\r\nHost: %s:%s\r\nUser-Agent: esp32_smart_module\r\nAccept:*/*\r\nAuthorization:Basic ZmlyZS1zYWZlOmFiY2QxMjM0\r\n\r\n", WEB_SERVER, WEB_PORT);

    //dns_initalize();
    memset (queuebuf, '\0', 256);
    uint8_t dns_retry_time = 0;
    http_task_count++;
    
    while(1) {
        ESP_LOGE (TAG, "HTTP http_task_count: %d", http_task_count);
        int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

        if(err != 0 || res == NULL) {
            ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
            if (++dns_retry_time > 10)
            {
                goto end;
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }

        /* Code to print the resolved IP.
           Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
        addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;
        ESP_LOGI(TAG, "DNS lookup succeeded. IP=%s", inet_ntoa(*addr));

        s = socket(res->ai_family, res->ai_socktype, 0);
        if(s < 0) {
            ESP_LOGE(TAG, "... Failed to allocate socket.");
            freeaddrinfo(res);
            if (++dns_retry_time > 10)
            {
                goto end;
            }
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... allocated socket");

        if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
            ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
            close(s);
            freeaddrinfo(res);
            if (++dns_retry_time > 10)
            {
                goto end;
            }
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }

        ESP_LOGI(TAG, "... connected");
        freeaddrinfo(res);

        if (write(s, REQUEST, strlen(REQUEST)) < 0) {
            ESP_LOGE(TAG, "... socket send failed");
            close(s);
            if (++dns_retry_time > 10)
            {
                goto end;
            }
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... socket send success");

        struct timeval receiving_timeout;
        receiving_timeout.tv_sec = 5;
        receiving_timeout.tv_usec = 0;
        if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
                sizeof(receiving_timeout)) < 0) {
            ESP_LOGE(TAG, "... failed to set socket receiving timeout");
            close(s);
            if (++dns_retry_time > 10)
            {
                goto end;
            }
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI(TAG, "... set socket receiving timeout success");

        static int index = 0;
        /* Read HTTP response */
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                //putchar(recv_buf[i]);
                queuebuf [index++] = recv_buf[i];
            }
        } while(r > 0);

        index = 0;
        
        putchar('\n');
        if (strstr (queuebuf, "200 OK") == NULL)
        {
           //vTaskDelay (2000/portTICK_RATE_MS);
            //continue;
            ESP_LOGI (TAG, "STRSTR NOT FOUND 200 STATUS CODE");
            goto wrong_response;
        }
        else
        {
            ESP_LOGI (TAG, "STRSTR FOUND 200 STATUS CODE");    
        }
        ESP_LOGD(TAG, "queue here: %s ", queuebuf);
        char *test = strstr (queuebuf, "{");
        if (test)
        {
            cJSON* server_ip = NULL;
            cJSON* port = NULL;
            cJSON* username = NULL;
            cJSON* password = NULL;
            cJSON* json_test = NULL;
            ESP_LOGI (TAG, "STRSTR FOUND {");
            json_test = parse_json (test);
            if (json_test)
            {
                server_ip = cJSON_GetObjectItemCaseSensitive (json_test, "ServerIp");
                if (cJSON_IsString(server_ip))
                {
                    ESP_LOGD(TAG,"server:\"%s\"\n", server_ip->valuestring);
                    memcpy (mqtt_info.server_ip, server_ip->valuestring, strlen (server_ip->valuestring));
                }

                port = cJSON_GetObjectItemCaseSensitive (json_test, "MqttPort");
                if (cJSON_IsString(port))
                {
                    ESP_LOGI(TAG,"port:\"%s\"\n", port->valuestring);
                    int port_index = gsm_utilities_get_number_from_string(0, port->valuestring, NULL);
                    mqtt_info.port = port_index;
                }
            
                username = cJSON_GetObjectItemCaseSensitive (json_test, "MqttUserName");
                if (cJSON_IsString(username))
                {
                    ESP_LOGD(TAG,"username:\"%s\"\n", username->valuestring);
                    memcpy (mqtt_info.username, username->valuestring, strlen (username->valuestring));
                }
                
                password = cJSON_GetObjectItemCaseSensitive (json_test, "MqttPassWord");
                if (cJSON_IsString(password))
                {
                    ESP_LOGD(TAG,"password:\"%s\"\n", password->valuestring);
                    memcpy (mqtt_info.password, password->valuestring, strlen (password->valuestring));
                }
                if (json_test != NULL)
                {
                    xQueueSend (mqtt_info_queue, &mqtt_info, 0);
                    cJSON_Delete(json_test);
                }
            }
                        
        }
        ESP_LOGI(TAG, "... done reading from socket. Last read return=%d errno=%d.", r, errno);
        wrong_response:
        close(s);
        ESP_LOGI(TAG, "DELETE TASK AFTER GET INFO!");
        end:
        http_task_alive = false;
        http_task_count--;
        vTaskDelete(NULL);
        // for(int countdown = 10; countdown >= 0; countdown--) {
        //     ESP_LOGI(TAG, "%d... ", countdown);
        //     vTaskDelay(1000 / portTICK_RATE_MS);
        //     // maybe we need vTaskDelete(); here!!!!
        // }
        
    }

}