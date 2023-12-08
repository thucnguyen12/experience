#include "mqtt_task.h"
#include "nvs_flash_app.h"
#include "esp_log.h"

// static const int CONNECT_BIT = BIT0;
// static const int STOP_BIT = BIT1;
//static const int GOT_DATA_BIT = BIT2;
static const int MQTT_DIS_CONNECT_BIT = BIT3;
static const int UPDATE_BIT = BIT4; 
static const int MQTT_CONNECT_BIT = BIT5;
static const int MQTT_ERROR_BIT = BIT6;
TaskHandle_t http_get_task_handler;
char mac_str[13];
static const char* TAG = "SECRET";

bool turn_off_log = true;

typedef enum
{
    MQTT_START_HTTP_GET,
    MQTT_WAIT_FOR_HTTP_CONNECT,
    MQTT_GOT_SERVER_INFO,
    MQTT_CONNECTING,
    MQTT_CONNECTED,
    MQTT_WHEN_LOST_CONNECTION,
    MQTT_DISCONNECTED
} mqtt_state_t;


static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event);
QueueHandle_t mqtt_info_queue;
esp_mqtt_client_config_t mqtt_config;// = {
        //     .uri = BROKER_URL,
        //     .event_handle = mqtt_event_handler,
        // };
esp_mqtt_client_handle_t mqtt_client = NULL;
static char mqtt_host [64];
int mqtt_port;
static char mqtt_username [64];
static char mqtt_password [64];

extern bool eth_started;
extern bool wifi_started;
static bool mqtt_server_ready = false;
bool http_get_ok = false;
bool http_get_task_existed = false;

mqtt_state_t mqtt_state = MQTT_START_HTTP_GET;
char string_key[32];
char login_reson_buff [32];
uint32_t length_of_login_reson = 0;
uint8_t wait_sim_timeout = 0;
bool http_task_alive = false;
mqtt_info_struct mqtt_broker_str;
static EventGroupHandle_t event_group = NULL;

extern test_info_t test_info_now;
void subcribe_mqtt_topic (void)
{
    int msg_id = 0;
    msg_id = esp_mqtt_client_subscribe(mqtt_client, "/AVmotor", 0);
}
extern void lock_device (void);
extern void unlock_device (void);
void creat_rtos_resource (void)
{
    mqtt_info_queue = xQueueCreate(1, sizeof (mqtt_info_struct));
    event_group = xEventGroupCreate();
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    
    esp_mqtt_client_handle_t client = event->client;
    int msg_id = 0;
    char string_key[32];
    // static uint16_t data_sensor_count_now = 1;
    // static uint16_t byte_read_from_flash;
    static int res;
    // static uint8_t sensor_data_process_now = 0;

    switch (event->event_id) {
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xEventGroupSetBits(event_group, MQTT_CONNECT_BIT);
        xEventGroupClearBits(event_group, MQTT_DIS_CONNECT_BIT);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGE(TAG, "MQTT_EVENT_DISCONNECTED");
        xEventGroupSetBits(event_group, MQTT_DIS_CONNECT_BIT);
        break;
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        // if (sensor_data_table[sensor_data_process_now].is_data_need_to_be_sent)
        // {
        //     memset (mqtt_payload, '\0', sizeof (mqtt_payload));
        //     make_sensor_info_payload (sensor_data_table[sensor_data_process_now].sensor_data_now, (char *)mqtt_payload);
        //     esp_mqtt_client_publish (mqtt_client, sensor_topic_header, (char*) mqtt_payload, 0, 1, 0); //only send when network is started
        //     sensor_data_table[sensor_data_process_now].is_data_need_to_be_sent = false;
        // }
        // sensor_data_process_now++;
        // if (sensor_data_process_now >= 100)
        // {
        //     sensor_data_process_now = 0;
        // }
        //msg_id = esp_mqtt_client_publish(client, "/topic/esp-pppos", "esp32-pppos", 0, 0, 0);
        //ESP_LOGD(TAG, "sent publish successful, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
     
        // if (just_conected_now)
        // {
        //     byte_read_from_flash = sizeof (sensor_info_in_flash_t);

        //     internal_flash_nvs_get_u16 (NVS_SENSOR_DATA_CNT, &data_sensor_count_in_flash);
        //     ESP_LOGE (TAG, "DATA IN FLASH NOW: %d", data_sensor_count_in_flash);
        //     //for (uint8_t i = 1; i < data_sensor_count_in_flash + 1; i++)
        //     if (data_sensor_count_now < data_sensor_count_in_flash + 1)
        //     {
        //         // for (uint8_t i = 0; i < 5; i++)
        //         // {
        //             ESP_LOGI (TAG, "SERVER STATE: %d, header state: %d, count now:%d", mqtt_server_ready, header_subscribed, data_sensor_count_now);
        //             if (mqtt_server_ready && header_subscribed && (data_sensor_count_now <= data_sensor_count_in_flash))
        //             {
        //                 sprintf (data_sensor_key, NVS_DATA_SENSOR, data_sensor_count_now);
        //                 read_data_from_flash (&data_sensor_store_in_flash, &byte_read_from_flash, data_sensor_key);
        //                 make_sensor_info_payload (data_sensor_store_in_flash.sensor_info, (char *)mqtt_payload); // Bo truong temper va fireZone
        //                 ESP_LOGI (TAG, "FLASH PAYLOAD NOW: %s", mqtt_payload);
                        
        //                 res = esp_mqtt_client_publish (mqtt_client, sensor_topic_header, mqtt_payload, 0, 1, 0);
        //                 ESP_LOGI (TAG, "PUBLISH NOW, res = %d",  res);
        //                 if (res != -1)
        //                 {
        //                     data_sensor_count_now++;
        //                 }

        //             }
                    
        //             if (data_sensor_count_now > data_sensor_count_in_flash)
        //             {
        //                 break;
        //             }
        //         // }
        //     }
        //     else 
        //     {
               
        //         data_sensor_count_now = 1;
        //         data_sensor_count_in_flash = 0; // reset after send all data in flash
        //         internal_flash_nvs_write_u16 (NVS_SENSOR_DATA_CNT, data_sensor_count_in_flash);
        //     }
        // }
        // else 
        // {
            
        //     // for (uint8_t i = 0; i < 100; i++)
        //     // {
        //         if (sensor_data_table[sensor_data_process_now].is_data_need_to_be_sent)
        //         {
        //             memset (mqtt_payload, '\0', sizeof (mqtt_payload));
        //             make_sensor_info_payload (sensor_data_table[sensor_data_process_now].sensor_data_now, (char *)mqtt_payload);
        //             esp_mqtt_client_publish (mqtt_client, sensor_topic_header, (char*) mqtt_payload, 0, 1, 0); //only send when network is started
        //             sensor_data_table[sensor_data_process_now].is_data_need_to_be_sent = false;
        //         }
        //         sensor_data_process_now++;
        //         if (sensor_data_process_now >= 100)
        //         {
        //             sensor_data_process_now = 0;
        //         }
        //     // }
        // }

        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        if (!turn_off_log)
        {
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
        }
       

        if (strstr (event->topic, "/AVmotor"))
        {
            if (strstr (event->data, "lock_device=1"))
            {
                printf ("lock device now!!!!!!!!!!!");
                
                lock_device();
            }
            else if (strstr (event->data, "lock_device=0"))
            {
                printf ("unlock now!!!!!!!!!!!!!!!!");
                unlock_device();
            }
        }

        break;
    case MQTT_EVENT_ERROR:
        xEventGroupSetBits(event_group, MQTT_ERROR_BIT);
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    default:
        ESP_LOGI(TAG, "MQTT other event id: %d", event->event_id);
        break;
    }
    return ESP_OK;
}


void mqtt_manager_task (void)
{
    BaseType_t res;
    //int response = 0;
    EventBits_t uxBits = 0;
    EventBits_t ubits = 0;
    static uint8_t wait_time = 0;
    static uint32_t heartbeart_cnt = 0;
    static bool need_send_device_info = false;
    
    static uint8_t ping_count_interval = 0;
    static uint8_t fire_count_interval = 0;
    static uint8_t subscribe_interval = 0;
    subscribe_interval++;
    ping_count_interval++;
    fire_count_interval++;
    // server_disconnect_monitor++;

    // if (server_disconnect_monitor > (15*60))
    // {
    //     ESP_LOGE (TAG, "SERVER DISCONNECT FOR A LONG TIME, RESTART");
    //     esp_restart ();
    // }

    uxBits = 0;

    switch (mqtt_state)
    {
    case MQTT_START_HTTP_GET:
        // obtain_mqtt_addr_by_http (true);
        if (wifi_started || eth_started)
        {
            if (http_task_alive == false)
            {
                http_task_alive = true;
                ESP_LOGW (TAG, "CREATE  HTTP TASK!!");
                xTaskCreate(&http_get_task, "http_get_task", 4096, NULL, 5, &http_get_task_handler);
            }
            
            //mqtt_state = MQTT_WAIT_FOR_HTTP_CONNECT;
            mqtt_state = MQTT_WAIT_FOR_HTTP_CONNECT; 
        }
        else
        {
            mqtt_state = MQTT_WHEN_LOST_CONNECTION;
        }

        break;
    case MQTT_WAIT_FOR_HTTP_CONNECT:
        //obtain_mqtt_addr_by_http (false);
        res = xQueueReceive (mqtt_info_queue, &mqtt_broker_str, 200/ portTICK_RATE_MS);
        wait_time++;
        if (res == pdTRUE)
        {
            ESP_LOGI (TAG, "Recieve queue\r\n");
            memcpy (mqtt_host, mqtt_broker_str.server_ip, strlen (mqtt_broker_str.server_ip));
            mqtt_port = mqtt_broker_str.port;
            memcpy (mqtt_username, mqtt_broker_str.username, strlen (mqtt_broker_str.username));
            memcpy (mqtt_password, mqtt_broker_str.password, strlen (mqtt_broker_str.password));
            // WRITE DATA INTO Flash STORAGE
#warning "can so sanh truoc khi ghi"
            write_mqtt_infor_from_flash (mqtt_host, mqtt_port, mqtt_username, mqtt_password);
            // process_config_data (STRING_TYPE, MQTT_ADDR, mqtt_broker_str.server_ip, 0, 0);
            // process_config_data (INT_TYPE, MQTT_PORT, NULL, mqtt_broker_str.port, 0);
            // process_config_data (STRING_TYPE, MQTT_USER, mqtt_broker_str.username, 0, 0);
            // process_config_data (STRING_TYPE, MQTT_PW, mqtt_broker_str.password, 0, 0);
            http_get_ok = true;
            mqtt_state = MQTT_GOT_SERVER_INFO;
        }
        else if (res == pdFALSE)
        {   
            //esp_restart();
            // memcpy (mqtt_host, config_infor_now.mqtt_add, strlen (config_infor_now.mqtt_add));
            // mqtt_port = config_infor_now.mqtt_port;
            // memcpy (mqtt_username, config_infor_now.mqtt_user, strlen (config_infor_now.mqtt_user));
            // memcpy (mqtt_password, config_infor_now.mqtt_user, strlen (config_infor_now.mqtt_user));
            ESP_LOGI (TAG, "Recieve queue fail, use last mqtt address\r\n");
            read_mqtt_infor_from_flash (mqtt_host,  &mqtt_port, mqtt_username, mqtt_password);
            http_get_ok = false;
            if ( wait_time++ > 3)
            {
                mqtt_state = MQTT_GOT_SERVER_INFO;
            }
        }
        break;
    case MQTT_GOT_SERVER_INFO:
        ESP_LOGW (TAG, "STATE GOT INFO!!!!!!!!!!!!");
        if (!http_get_ok)
        {
            res = xQueueReceive (mqtt_info_queue, &mqtt_broker_str, 0);
            if (res == pdTRUE)
            {
                ESP_LOGI (TAG, "Recieve queue\r\n");
                memcpy (mqtt_host, mqtt_broker_str.server_ip, strlen (mqtt_broker_str.server_ip));
                mqtt_port = mqtt_broker_str.port;
                memcpy (mqtt_username, mqtt_broker_str.username, strlen (mqtt_broker_str.username));
                memcpy (mqtt_password, mqtt_broker_str.password, strlen (mqtt_broker_str.password));
                // WRITE DATA INTO Flash STORAGE
                write_mqtt_infor_from_flash (mqtt_host, mqtt_port, mqtt_username, mqtt_password);
                // process_config_data (STRING_TYPE, MQTT_ADDR, mqtt_broker_str.server_ip, 0, 0);
                // process_config_data (INT_TYPE, MQTT_PORT, NULL, mqtt_broker_str.port, 0);
                // process_config_data (STRING_TYPE, MQTT_USER, mqtt_broker_str.username, 0, 0);
                // process_config_data (STRING_TYPE, MQTT_PW, mqtt_broker_str.password, 0, 0);
                http_get_ok = true;
            }
        }
        mqtt_config.host = mqtt_host;
        mqtt_config.port = mqtt_port;
        mqtt_config.username = mqtt_username;
        mqtt_config.password = mqtt_password;
        mqtt_config.event_handle = mqtt_event_handler;
        mqtt_config.disable_auto_reconnect = false;
        // mqtt_config.reconnect_timeout_ms = 600000;
        
        // if (!first_time)
        // {
        //     esp_mqtt_client_destroy (mqtt_client);
        // }
        // if (mqtt_client != NULL)
        // {
        //     esp_mqtt_client_destroy (mqtt_client);
        // }

        if (mqtt_client == NULL)
        {
            mqtt_client = esp_mqtt_client_init(&mqtt_config);
            ESP_LOGI (TAG, "MQTT INIT");
            esp_mqtt_client_start(mqtt_client);
        }

        
        mqtt_state = MQTT_CONNECTING;
        break;
    case MQTT_CONNECTING:
        ESP_LOGW (TAG, "STATE CONNECTING");

        uxBits = xEventGroupWaitBits(event_group, MQTT_CONNECT_BIT | MQTT_ERROR_BIT, pdTRUE, pdFALSE, 0);
        if ((uxBits & MQTT_CONNECT_BIT) == MQTT_CONNECT_BIT)
        {
            ESP_LOGW (TAG, "MQTT CONNECTED!!!!!");
            need_send_device_info = true;

            mqtt_server_ready = true;
            subcribe_mqtt_topic ();
            // if (got_gsm_imei)
            // {   
            //     if (got_sim_imei && need_send_device_info)
            //     {
            //         length_of_login_reson = sizeof (login_reson_buff);
            //         if (internal_flash_nvs_read_string (LOG_IN_RESON, login_reson_buff, length_of_login_reson) == ESP_OK)
            //         {
            //             memcpy (device_info.loginReson, login_reson_buff, sizeof (login_reson_buff));
            //             internal_flash_nvs_write_string (LOG_IN_RESON, LOGIN_RESON_DEFAULT);
            //         }
            //         memcpy (device_info.imei, GSM_IMEI_IN_FLASH, sizeof (GSM_IMEI_IN_FLASH));
            //         memcpy (device_info.simIMEI, SIM_IMEI, sizeof(SIM_IMEI));
            //         sprintf(device_info.firmware, "SFUL03-%s", FIRMWARE_VERSION);
            //         sprintf(device_info.hardwareVersion, "SFUL03-%s", HARDWARE_VERSION);
            //         sprintf(device_info.ExpFwVersion, "SFUL03-%d", (uint8_t) gd32_version.firmware);
            //         sprintf(device_info.ExpHwVersion, "SFUL03-%d", (uint8_t) gd32_version.hardware);
            //         device_info.updateTime = app_time ();
            //         make_device_info_payload (device_info, mqtt_payload);
            //         if (header_subscribed)
            //         esp_mqtt_client_publish(mqtt_client, info_topic_header, (char*)mqtt_payload, 0, 1, 0);
            //         need_send_device_info = false;

            //         if (memcmp (GSM_IMEI, GSM_IMEI_IN_FLASH, 16))
            //         {
            //             process_config_data(STRING_TYPE, GSM_IMEI_FLASH, GSM_IMEI, 0,0);
            //         }
            //         subcribe_mqtt_topic ();
            //     }
                      
            // }
            
            ESP_LOGW (TAG, "SEND DEVICE INFO NOW");
            // truoc do can doc cau hinh ra tu flash
            ESP_LOGI (TAG, "READ CONFIG DATA FROM FLASH");
            // read_data_from_flash (&config_infor_now, sizeof (info_config_t), NVS_CONFIG_KEY);
            // send_current_config (config_infor_now);
            
            esp_err_t err = ESP_OK;
            // for (mqtt_config_list i = 0; i < MAX_CONFIG_HANDLE; i++)
            // {
            //     static type_of_mqtt_data_t type_of_process_data;
            //     // ESP_LOGI (TAG, "READ 1");
            //     type_of_process_data = get_type_of_data (i);
            //     // ESP_LOGI (TAG, "READ 2");
            //     err = read_config_data_from_flash (&config_infor_now, type_of_process_data, i);
            //     // ESP_LOGI (TAG, "READ 3");
            //     if (err != ESP_OK)
            //     {
            //         make_key_to_store_type_in_nvs (string_key, i);
            //         ESP_LOGI (TAG, "GOT ERR %d WHILE READING %s KEY", err, string_key);
            //     }
            // }
            // ESP_LOGI (TAG, "READ CONFIG DATA FROM FLASH AND SEND IT");
            // send_current_config (config_infor_now);
           
            mqtt_state = MQTT_CONNECTED;
        }
        else if ((uxBits & MQTT_ERROR_BIT) == MQTT_ERROR_BIT)
        {
            mqtt_state = MQTT_DISCONNECTED;
        }
        ESP_LOGI (TAG, "RECEIVE QUEUE");
        // res = xQueueReceive (min_payload_queue, &payload_buffer_queue_rev, 200/ portTICK_RATE_MS);
    
        if (res  == pdTRUE)  //o day la chua co ket noi
        {
            // process_sensor_data_info (&sensor_info, payload_buffer_queue_rev);
            // handle_sensor_data_while_lost_connection (&sensor_info);
        }
        break;
    case MQTT_CONNECTED:
        ESP_LOGW (TAG, "STATE CONNECTED");
        if (!turn_off_log)
        {
            ESP_LOGW (TAG,"MQTT host NOW:%s\r\n", mqtt_config.host);
            ESP_LOGW (TAG,"MQTT port NOW:%d\r\n", mqtt_config.port);
            ESP_LOGW (TAG,"MQTT username NOW:%s\r\n", mqtt_config.username);
            ESP_LOGW (TAG,"MQTT password NOW:%s\r\n", mqtt_config.password);
        }
        // process sensor data info
        // server_disconnect_monitor = 0;
        // if (got_gsm_imei)
        // {
        //     internal_flash_nvs_read_string (INTERNAL_FLASH_NVS_KEY_INPUT_GSM_IMEI, GSM_IMEI_IN_FLASH ,sizeof (GSM_IMEI_IN_FLASH));
        //     // mqtt_config_list config = GSM_IMEI_FLASH;
        //     if (memcmp (GSM_IMEI, GSM_IMEI_IN_FLASH, 16))
        //     {
        //         process_config_data(STRING_TYPE, GSM_IMEI_FLASH, GSM_IMEI, 0,0);
        //         ESP_LOGI (TAG, "subscribe mqtt topic"); 
        //         subcribe_mqtt_topic ();
        //     }
            
        // }
        
        //gui mac len server


        //resub
        if (subscribe_interval > 600)
        {
            // if (got_gsm_imei)
            // {
                ESP_LOGD (TAG, "subscribe mqtt topic again"); 
                subcribe_mqtt_topic ();
            // }

            subscribe_interval = 0;
        }
        else
        {
            static uint8_t interval = 0;
            if (interval ++  > 5)
            {
                sprintf (mac_str, "%02x%02x%02x%02x%02x%02x", test_info_now.mac[0], 
                                                                test_info_now.mac[1],
                                                                test_info_now.mac[2], 
                                                                test_info_now.mac[3],
                                                                test_info_now.mac[4],
                                                                test_info_now.mac[5]);
                esp_mqtt_client_publish (mqtt_client, "/AVmotor", mac_str, 0, 1, 0); //only send when network is started
            }
            
        }

        // check disconnected bit
        uxBits = 0;
        uxBits = xEventGroupWaitBits(event_group, MQTT_DIS_CONNECT_BIT | UPDATE_BIT, pdTRUE, pdFALSE, 0);
        if ((uxBits & MQTT_DIS_CONNECT_BIT) == MQTT_DIS_CONNECT_BIT)
        {
            ESP_LOGW (TAG, "MQTT DISCONNECTED BIT : %d", uxBits);
            mqtt_state = MQTT_WHEN_LOST_CONNECTION;
        }
        // check ota update bit
        // ubits = xEventGroupWaitBits (event_group, UPDATE_BIT, pdTRUE, pdTRUE, 0/ portTICK_RATE_MS); 

        // if ((ubits & UPDATE_BIT) == UPDATE_BIT)
        // {
        //     //tao task cap nhat ota
            
        //     if (!ota_now)
        //     {
        //         internal_flash_nvs_write_string (LOG_IN_RESON, "Update_ota");
        //         xTaskCreate (&advanced_ota_example_task, "advanced_ota_example_task", 1024 * 8, NULL, 5, NULL);
        //         ota_now = true;
        //     }
            
        // }

        if (!(wifi_started || eth_started))
        {
            mqtt_state = MQTT_WHEN_LOST_CONNECTION;
        }

        break;
    case MQTT_WHEN_LOST_CONNECTION:
        ESP_LOGW (TAG, "MQTT_WHEN_LOST_CONNECTION");
        // res = xQueueReceive (min_payload_queue, &payload_buffer_queue_rev, 200/ portTICK_RATE_MS);
        if (res  == pdTRUE)  //o day la chua co ket noi
        {
            // process_sensor_data_info (&sensor_info, payload_buffer_queue_rev);
            // handle_sensor_data_while_lost_connection (&sensor_info);
        }
        
        if (wifi_started || eth_started)
        {
            mqtt_state = MQTT_START_HTTP_GET;
        }

        break;
    case MQTT_DISCONNECTED:
        // if (mqtt_client != NULL)
        // {
        //     esp_mqtt_client_destroy (mqtt_client);
        // }
        mqtt_state = MQTT_START_HTTP_GET;
        break;
    default:
        break;
    }
}