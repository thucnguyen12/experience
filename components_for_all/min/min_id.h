#ifndef MIN_ID_H
#define MIN_ID_H

/*
    Put applation id here
    #define MIN_ID_GET_GPIO                     0x00
    #define MIN_ID_SET_GPIO                     0x01
    #define MIN_ID_UPDATE_ALL                   0x02
    #define MIN_ID_PING                         0x03
    #define MIN_ID_BUTTON_ISR                   0x04
    #define MIN_ID_SLAVE_RESET                  0x05
    #define MIN_ID_RESET						0x06
    #define MIN_ID_OTA_UPDATE_START             0x07
    #define MIN_ID_OTA_UPDATE_TRANSFER          0x08
    #define MIN_ID_OTA_UPDATE_END               0x09
    #define MIN_ID_OTA_ACK                      0x0A
    #define MIN_ID_OTA_FAILED                   0x0B
    #define MIN_ID_BUFFER_FULL                  0x0C
    #define MIN_ID_MASTER_OTA_BEGIN             0x0D
    #define MIN_ID_MASTER_OTA_END               0x0E
    #define MIN_ID_RS485_FORWARD                0x0F
    #define MIN_ID_RS232_FORWARD                0x10

	#define MIN_ID_TEST_WATCHDOG                0x27

	#define MIN_ID_RS232_ENTER_TEST_MODE        0x28

	#define MIN_ID_RS232_ESP32_RESET            0x29

	#define MIN_ID_SEND_TIMESTAMP				0x26
*/

//PING CONTROL ALIVE
#define MIN_ID_PING_RESPONSE 0x20
#define MIN_ID_ESP32_STARTED 0x21
#define MIN_ID_GD32_RESET 0x22
#define MIN_ID_PING_ESP_DEAD 0x25 // this only for test
#define MIN_ID_PING_ESP_ALIVE 0x10

//PING COMMUNICATE
#define MIN_ID_SEND_SPI_FROM_ESP32 0x11
#define MIN_ID_RECIEVE_SPI_FROM_GD32 0x12
#define MIN_ID_SEND_AND_RECEIVE_HEARTBEAT_MSG 0x13
#define MIN_ID_RECEIVE_HEARTBEAT_MSG 0x14
#define MIN_ID_SEND_AND_RECEIVE_BEACON_MSG 0x15
#define MIN_ID_TEMPER_INFO_MSG  0x16
#define MIN_ID_SEND_KEY_CONFIG 017
#define MIN_ID_NEW_SENSOR_PAIRING 0x18
#define MIN_ID_ZONE_STATE_NOW 0x19

//OTA UPDATE
#define MIN_ID_GD32_OTA_BEGIN 0x00
#define MIN_ID_GD32_OTA_DATA 0x01
#define MIN_ID_GD32_OTA_END 0x02

#define MIN_ID_GD32_VERSION 0x03
//PING TIME
#define MIN_ID_TIMESTAMP 0x30

//GPIO CONTROL
#define MIN_ID_GPIO_CONTROL 0X40
#define MIN_ID_POWER_STATUS 0x31
#define MIN_ID_REPORT_ERROR 0x32
#define MIN_ID_SEND_CABIN_MODE 0x33
#define MIN_ID_ERR_ZONE 0x34

// MIN MSG FOR APP CLI
#define MIN_ID_MAKE_ZONE_ALARM 0x50
#define MIN_ID_MAKE_BEACON_ALARM 0x51
#define MIN_ID_MAKE_ZONE_ERROR 0x52



#endif /* MIN_ID_H */
