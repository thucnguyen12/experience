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


mqtt_state_t mqtt_state = MQTT_START_HTTP_GET;
char string_key[32];
char login_reson_buff [32];
uint32_t length_of_login_reson = 0;
uint8_t wait_sim_timeout = 0;
bool ota_now = false;
bool http_task_alive = false;
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
    server_disconnect_monitor++;

    if (server_disconnect_monitor > (15*60))
    {
        ESP_LOGE (TAG, "SERVER DISCONNECT FOR A LONG TIME, RESTART");
        esp_restart ();
    }

    uxBits = 0;

    switch (mqtt_state)
    {
    case MQTT_START_HTTP_GET:
        // obtain_mqtt_addr_by_http (true);
        if (gsm_started || wifi_started || eth_started)
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
            process_config_data (STRING_TYPE, MQTT_ADDR, mqtt_broker_str.server_ip, 0, 0);
            process_config_data (INT_TYPE, MQTT_PORT, NULL, mqtt_broker_str.port, 0);
            process_config_data (STRING_TYPE, MQTT_USER, mqtt_broker_str.username, 0, 0);
            process_config_data (STRING_TYPE, MQTT_PW, mqtt_broker_str.password, 0, 0);
            http_get_ok = true;
            mqtt_state = MQTT_GOT_SERVER_INFO;
        }
        else if (res == pdFALSE)
        {   
            //esp_restart();
            memcpy (mqtt_host, config_infor_now.mqtt_add, strlen (config_infor_now.mqtt_add));
            mqtt_port = config_infor_now.mqtt_port;
            memcpy (mqtt_username, config_infor_now.mqtt_user, strlen (config_infor_now.mqtt_user));
            memcpy (mqtt_password, config_infor_now.mqtt_user, strlen (config_infor_now.mqtt_user));
            ESP_LOGI (TAG, "Recieve queue fail, use last mqtt address\r\n");
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
                process_config_data (STRING_TYPE, MQTT_ADDR, mqtt_broker_str.server_ip, 0, 0);
                process_config_data (INT_TYPE, MQTT_PORT, NULL, mqtt_broker_str.port, 0);
                process_config_data (STRING_TYPE, MQTT_USER, mqtt_broker_str.username, 0, 0);
                process_config_data (STRING_TYPE, MQTT_PW, mqtt_broker_str.password, 0, 0);
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
        ESP_LOGW (TAG,"MQTT host NOW:%s\r\n", mqtt_config.host);
        ESP_LOGW (TAG,"MQTT port NOW:%d\r\n", mqtt_config.port);
        ESP_LOGW (TAG,"MQTT username NOW:%s\r\n", mqtt_config.username);
        ESP_LOGW (TAG,"MQTT password NOW:%s\r\n", mqtt_config.password);
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
            if (got_gsm_imei)
            {   
                if (got_sim_imei && need_send_device_info)
                {
                    length_of_login_reson = sizeof (login_reson_buff);
                    if (internal_flash_nvs_read_string (LOG_IN_RESON, login_reson_buff, length_of_login_reson) == ESP_OK)
                    {
                        memcpy (device_info.loginReson, login_reson_buff, sizeof (login_reson_buff));
                        internal_flash_nvs_write_string (LOG_IN_RESON, LOGIN_RESON_DEFAULT);
                    }
                    memcpy (device_info.imei, GSM_IMEI_IN_FLASH, sizeof (GSM_IMEI_IN_FLASH));
                    memcpy (device_info.simIMEI, SIM_IMEI, sizeof(SIM_IMEI));
                    sprintf(device_info.firmware, "SFUL03-%s", FIRMWARE_VERSION);
                    sprintf(device_info.hardwareVersion, "SFUL03-%s", HARDWARE_VERSION);
                    sprintf(device_info.ExpFwVersion, "SFUL03-%d", (uint8_t) gd32_version.firmware);
                    sprintf(device_info.ExpHwVersion, "SFUL03-%d", (uint8_t) gd32_version.hardware);
                    device_info.updateTime = app_time ();
                    make_device_info_payload (device_info, mqtt_payload);
                    if (header_subscribed)
                    esp_mqtt_client_publish(mqtt_client, info_topic_header, (char*)mqtt_payload, 0, 1, 0);
                    need_send_device_info = false;

                    if (memcmp (GSM_IMEI, GSM_IMEI_IN_FLASH, 16))
                    {
                        process_config_data(STRING_TYPE, GSM_IMEI_FLASH, GSM_IMEI, 0,0);
                    }
                    subcribe_mqtt_topic ();
                }
                      
            }
            
            ESP_LOGW (TAG, "SEND DEVICE INFO NOW");
            // truoc do can doc cau hinh ra tu flash
            ESP_LOGI (TAG, "READ CONFIG DATA FROM FLASH");
            // read_data_from_flash (&config_infor_now, sizeof (info_config_t), NVS_CONFIG_KEY);
            // send_current_config (config_infor_now);
            
            esp_err_t err = ESP_OK;
            for (mqtt_config_list i = 0; i < MAX_CONFIG_HANDLE; i++)
            {
                static type_of_mqtt_data_t type_of_process_data;
                // ESP_LOGI (TAG, "READ 1");
                type_of_process_data = get_type_of_data (i);
                // ESP_LOGI (TAG, "READ 2");
                err = read_config_data_from_flash (&config_infor_now, type_of_process_data, i);
                // ESP_LOGI (TAG, "READ 3");
                if (err != ESP_OK)
                {
                    make_key_to_store_type_in_nvs (string_key, i);
                    ESP_LOGI (TAG, "GOT ERR %d WHILE READING %s KEY", err, string_key);
                }
            }
            ESP_LOGI (TAG, "READ CONFIG DATA FROM FLASH AND SEND IT");
            send_current_config (config_infor_now);
            just_conected_now = true;
            mqtt_state = MQTT_CONNECTED;
        }
        else if ((uxBits & MQTT_ERROR_BIT) == MQTT_ERROR_BIT)
        {
            mqtt_state = MQTT_DISCONNECTED;
        }
        ESP_LOGI (TAG, "RECEIVE QUEUE");
        res = xQueueReceive (min_payload_queue, &payload_buffer_queue_rev, 200/ portTICK_RATE_MS);
    
        if (res  == pdTRUE)  //o day la chua co ket noi
        {
            process_sensor_data_info (&sensor_info, payload_buffer_queue_rev);
            handle_sensor_data_while_lost_connection (&sensor_info);
        }
        break;
    case MQTT_CONNECTED:
        ESP_LOGW (TAG, "STATE CONNECTED");
        // process sensor data info
        server_disconnect_monitor = 0;
        if (got_gsm_imei)
        {
            internal_flash_nvs_read_string (INTERNAL_FLASH_NVS_KEY_INPUT_GSM_IMEI, GSM_IMEI_IN_FLASH ,sizeof (GSM_IMEI_IN_FLASH));
            // mqtt_config_list config = GSM_IMEI_FLASH;
            if (memcmp (GSM_IMEI, GSM_IMEI_IN_FLASH, 16))
            {
                process_config_data(STRING_TYPE, GSM_IMEI_FLASH, GSM_IMEI, 0,0);
                ESP_LOGI (TAG, "subscribe mqtt topic"); 
                subcribe_mqtt_topic ();
            }
            
        }
        res = xQueueReceive (min_payload_queue, &payload_buffer_queue_rev, 100 / portTICK_RATE_MS);

        if (res  == pdTRUE)  
        {
            ESP_LOGI (TAG, "THERE ARE INTERNET PUBLISH NOW");
            static uint8_t sensor_data_in_table = 0; 
            // ESP_LOGI (TAG, "server ok: %d, header ok: %d", mqtt_server_ready, header_subscribed);
            process_sensor_data_info (&sensor_info, payload_buffer_queue_rev);
            if (header_subscribed)
            {
                // ESP_LOGI (TAG, "PUBLISH PAYLOAD TO MQTT WITH HEADER: %s", sensor_topic_header);
                // esp_mqtt_client_publish (mqtt_client, sensor_topic_header, (char*) mqtt_payload, 0, 1, 0); //only send when network is started
                // sensor_data_table
                
                if (!just_conected_now)
                {
                    memset (mqtt_payload, '\0', sizeof (mqtt_payload));
                    make_sensor_info_payload (sensor_info, (char *)mqtt_payload);
                    //response =
                    esp_mqtt_client_publish (mqtt_client, sensor_topic_header, (char*) mqtt_payload, 0, 1, 0); //only send when network is started
                    // ESP_LOGW (TAG, "TOPIC HEADER: %s", sensor_topic_header);
                    // ESP_LOGW (TAG, "payload: %s", mqtt_payload);
                    // ESP_LOGW (TAG, "response: %d", response);

                }
                else 
                {
                    memcpy(&sensor_data_table [sensor_data_in_table].sensor_data_now, &sensor_info, sizeof (app_beacon_data_t));
                    sensor_data_table [sensor_data_in_table].is_data_need_to_be_sent = true;
                }
                
                sensor_data_in_table++;
                if (sensor_data_in_table >= 100)
                {
                    sensor_data_in_table = 0;
                }
            }
            else 
            {
                ESP_LOGW (TAG, "NOT SUBSCRIBE TOPIC YET");
                handle_sensor_data_while_lost_connection (&sensor_info);
            }
        }

        res = xQueueReceive (min_heartbeat_payload_queue, &alarm_queue_rev, 10/ portTICK_RATE_MS);
        if (res == pdTRUE)
        {
            ESP_LOGI (TAG, "RECEIVE QUEUE OK! PING COUNT INTERVAL:%d", ping_count_interval);
            static int last_fire_status = 0;
            if (alarm_queue_rev.fire_status.Value) //co bao dong 
            {
                if (mqtt_server_ready && (gsm_started || wifi_started || eth_started) && header_subscribed && (fire_count_interval > 9))
                {
                    ESP_LOGE (TAG, "THERE ARE FIRE: %d", alarm_queue_rev.fire_status.Value);
                    make_firealarm_payload (alarm_queue_rev, payload_buffer_queue_rev);
                    esp_mqtt_client_publish(mqtt_client, fire_alarm_topic_header, (char*)payload_buffer_queue_rev, 0, 1, 0);
                    //last_timestamp_ping = timestamp_now;
                    fire_count_interval = 0;
                    ESP_LOGE (TAG, "FIRE MSG PUBLISHED");
                }

                if (mqtt_server_ready && (gsm_started || wifi_started || eth_started) && header_subscribed && (ping_count_interval > 10))
                {
                    
                    make_fire_status_payload (alarm_queue_rev, (char *)payload_buffer_queue_rev); // Bo truong temper va fireZone
                    esp_mqtt_client_publish(mqtt_client, heart_beat_topic_header, (char*)payload_buffer_queue_rev, 0, 1, 0);
                    //last_timestamp_ping = timestamp_now;
                    ping_count_interval = 0;
                    ESP_LOGE (TAG, "HEARTBEAT MSG PUBLISHED");
                }
            }
            else // khong co bao dong
            {
                
                //ESP_LOGI (TAG, "RECEIVE PING QUEUE: %u - %u", timestamp_now, last_timestamp_ping);
                if (mqtt_server_ready && (gsm_started || wifi_started || eth_started) && header_subscribed && (ping_count_interval > 60))
                {
                    make_fire_status_payload (alarm_queue_rev, (char *)payload_buffer_queue_rev); // Bo truong temper va fireZone
                    esp_mqtt_client_publish(mqtt_client, heart_beat_topic_header, (char*)payload_buffer_queue_rev, 0, 1, 0);
                    //last_timestamp_ping = timestamp_now;
                    ping_count_interval = 0;
                    heartbeart_cnt++;
                    ESP_LOGE (TAG, "HEARTBEAT MSG PUBLISHED: %d", heartbeart_cnt);
                }
            }

            if (alarm_queue_rev.fire_status.Value != last_fire_status)
            {
                make_firealarm_payload (alarm_queue_rev, payload_buffer_queue_rev);
                esp_mqtt_client_publish(mqtt_client, fire_alarm_topic_header, (char*)payload_buffer_queue_rev, 0, 1, 0);
                last_fire_status = alarm_queue_rev.fire_status.Value;
            }
            
        }

        if (got_sim_imei && need_send_device_info)
        {
            
            memcpy (device_info.imei, GSM_IMEI_IN_FLASH, sizeof (GSM_IMEI_IN_FLASH));
            memcpy (device_info.simIMEI, SIM_IMEI, sizeof(SIM_IMEI));
            sprintf(device_info.firmware, "SFUL03-%s", FIRMWARE_VERSION);
            sprintf(device_info.hardwareVersion, "SFUL03-%s", HARDWARE_VERSION);
            sprintf(device_info.ExpFwVersion, "SFUL03-%d", (uint8_t) gd32_version.firmware);
            sprintf(device_info.ExpHwVersion, "SFUL03-%d", (uint8_t) gd32_version.hardware);
            device_info.updateTime = app_time ();
            make_device_info_payload (device_info, mqtt_payload);
            if (header_subscribed)
            esp_mqtt_client_publish(mqtt_client, info_topic_header, (char*)mqtt_payload, 0, 1, 0);
            need_send_device_info = false;
        }
        else
        {
            wait_sim_timeout++;
            if (wait_sim_timeout  > 30)
            {
                if (!got_sim_imei)
                {
                    ESP_LOGW (TAG, "FOR A LONG TIME CAN GET SIM IMEI, PUBLISH ANYWAY");
                
                    memcpy (device_info.imei, GSM_IMEI_IN_FLASH, sizeof (GSM_IMEI_IN_FLASH));
                    memcpy (device_info.simIMEI, SIM_IMEI, sizeof(SIM_IMEI));
                    sprintf(device_info.firmware, "SFUL03-%s", FIRMWARE_VERSION);
                    sprintf(device_info.hardwareVersion, "SFUL03-%s", HARDWARE_VERSION);
                    sprintf(device_info.ExpFwVersion, "SFUL03-%d", (uint8_t) gd32_version.firmware);
                    sprintf(device_info.ExpHwVersion, "SFUL03-%d", (uint8_t) gd32_version.hardware);
                    device_info.updateTime = app_time ();
                    make_device_info_payload (device_info, mqtt_payload);
                    if (header_subscribed)
                    esp_mqtt_client_publish(mqtt_client, info_topic_header, (char*)mqtt_payload, 0, 1, 0);
                    need_send_device_info = false;
                    wait_sim_timeout = 0;
                }

            }
        }

        //resub
        if (subscribe_interval > 90)
        {
            // if (got_gsm_imei)
            // {
                ESP_LOGI (TAG, "subscribe mqtt topic again"); 
                subcribe_mqtt_topic ();
            // }

            subscribe_interval = 0;
        }

        // check disconnected bit
        uxBits = 0;
        uxBits = xEventGroupWaitBits(event_group, MQTT_DIS_CONNECT_BIT | UPDATE_BIT, pdTRUE, pdFALSE, 0);
        if ((uxBits & MQTT_DIS_CONNECT_BIT) == MQTT_DIS_CONNECT_BIT)
        {
            ESP_LOGW (TAG, "MQTT DISCONNECTED BIT : %d", uxBits);
            internal_flash_nvs_write_string (LOG_IN_RESON,"Mqtt_disconected");
            mqtt_state = MQTT_WHEN_LOST_CONNECTION;
        }
        // check ota update bit
        // ubits = xEventGroupWaitBits (event_group, UPDATE_BIT, pdTRUE, pdTRUE, 0/ portTICK_RATE_MS); 

        if ((ubits & UPDATE_BIT) == UPDATE_BIT)
        {
            //tao task cap nhat ota
            
            if (!ota_now)
            {
                internal_flash_nvs_write_string (LOG_IN_RESON, "Update_ota");
                xTaskCreate (&advanced_ota_example_task, "advanced_ota_example_task", 1024 * 8, NULL, 5, NULL);
                ota_now = true;
            }
            
        }

        if (!(gsm_started || wifi_started || eth_started))
        {
            mqtt_state = MQTT_WHEN_LOST_CONNECTION;
        }

        break;
    case MQTT_WHEN_LOST_CONNECTION:
        ESP_LOGW (TAG, "MQTT_WHEN_LOST_CONNECTION");
        res = xQueueReceive (min_payload_queue, &payload_buffer_queue_rev, 200/ portTICK_RATE_MS);
        if (res  == pdTRUE)  //o day la chua co ket noi
        {
            process_sensor_data_info (&sensor_info, payload_buffer_queue_rev);
            handle_sensor_data_while_lost_connection (&sensor_info);
        }
        
        if (gsm_started || wifi_started || eth_started)
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