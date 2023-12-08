set(srcs "mqtt_task.c")

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "."
                    REQUIRES GPIO_config app_ble wireless_protocol tcp_socket json http_request nvs_flash_app test_produce mqtt app_update esp_http_client esp_https_ota)
