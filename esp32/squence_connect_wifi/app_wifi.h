/*
 * app_wifi.h
 *
 *  Created on: Apr 27, 2021
 *      Author: huybk
 */

#ifndef COMPONENTS_APP_WIFI_INCLUDE_APP_WIFI_H_
#define COMPONENTS_APP_WIFI_INCLUDE_APP_WIFI_H_



#include <stdint.h>
#include <stdbool.h>

#define WIFI_MAXIMUM_RETRY           10
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
#define WIFI_PING_SUCESS BIT2
#define WIFI_PING_TIMEOUT BIT3
// #define WIFI_DISCONNECTED_BIT BIT4

#define PING_SUCESS BIT4
#define PING_TIMEOUT BIT5

#define CONFIG_ESP_WIFI_SSID "HuyTV"
#define CONFIG_ESP_WIFI_PASSWORD "1234567890"

#define  WIFI_EVENT_DISCONNECT_MANUAL 23
#define  WIFI_EVENT_RECONNECT_MANUAL 24

typedef enum
{
    GSM_4G_PROTOCOL,
    WIFI_PROTOCOL,
    ETHERNET_PROTOCOL,
    PROTOCOL_NONE
} protocol_type;


/**
 * @brief			Connect to AP
 */
void app_wifi_connect(char *ssid, char *password);

/**
 * @brief			Get ip status
 */
bool app_wifi_is_ip_acquired(void);

#endif /* COMPONENTS_APP_WIFI_INCLUDE_APP_WIFI_H_ */
