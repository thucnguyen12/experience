#ifndef BOARD_HW_H
#define BOARD_HW_H

#include <stdint.h>

// gpio rs232
#define GPIO_RS232_PORT GPIOA
#define GPIO_RS232_RX_PIN GPIO_PIN_2
#define GPIO_RS232_TX_PIN GPIO_PIN_3

//gpio uart
#define GPIO_UART_PORT GPIOA
#define GPIO_ESP_RX_PIN GPIO_PIN_9
#define GPIO_ESP_TX_PIN GPIO_PIN_10

//gpio lcd
#define GPIO_EN_PWR_LCD_PORT GPIOC
#define GPIO_EN_PWR_LCD_PIN GPIO_PIN_13

#define GPIO_LCD_DI_PORT GPIOC
#define GPIO_LCD_DI_PIN GPIO_PIN_14

#define GPIO_LCD_RW_PORT GPIOC
#define GPIO_LCD_RW_PIN GPIO_PIN_15

#define GPIO_LCD_EN_PORT GPIOF
#define GPIO_LCD_EN_PIN GPIO_PIN_0

#define GPIO_LCD_D0_PORT GPIOF
#define GPIO_LCD_D0_PIN GPIO_PIN_1

#define GPIO_LCD_D1_PORT GPIOA
#define GPIO_LCD_D1_PIN GPIO_PIN_0

#define GPIO_LCD_D2_PORT GPIOA
#define GPIO_LCD_D2_PIN GPIO_PIN_1

#define GPIO_LCD_D3_PORT GPIOA
#define GPIO_LCD_D3_PIN GPIO_PIN_4

#define GPIO_LCD_D4_PORT GPIOA
#define GPIO_LCD_D4_PIN GPIO_PIN_7

#define GPIO_LCD_D5_PORT GPIOB
#define GPIO_LCD_D5_PIN GPIO_PIN_0

#define GPIO_LCD_D6_PORT GPIOB
#define GPIO_LCD_D6_PIN GPIO_PIN_1

#define GPIO_LCD_D7_PORT GPIOB
#define GPIO_LCD_D7_PIN GPIO_PIN_2

#define GPIO_LCD_RST_PORT GPIOB
#define GPIO_LCD_RST_PIN GPIO_PIN_10

#define GPIO_LCD_PWM_PORT GPIOB
#define GPIO_LCD_PWM_PIN GPIO_PIN_11

//spi
#define GPIO_SPI0_SCK_PORT GPIOB
#define GPIO_SPI0_SCK_PIN GPIO_PIN_3
#define GPIO_SPI0_MISO_PORT GPIOB
#define GPIO_SPI0_MISO_PIN GPIO_PIN_4
#define GPIO_SPI0_MOSI_PORT GPIOB
#define GPIO_SPI0_MOSI_PIN GPIO_PIN_5

//lora
#define GPIO_LORA_CS_PORT GPIOB
#define GPIO_LORA_CS_PIN GPIO_PIN_6
#define GPIO_LORA_IO0_PORT GPIOA
#define GPIO_LORA_IO0_PIN GPIO_PIN_5
#define GPIO_LORA_IO4_PORT GPIOB
#define GPIO_LORA_IO4_PIN GPIO_PIN_7
#define GPIO_LORA_EN_PORT GPIOB
#define GPIO_LORA_EN_PIN GPIO_PIN_8

//nrf
#define GPIO_NRF_CS_PORT GPIOB
#define GPIO_NRF_CS_PIN GPIO_PIN_9
#define GPIO_ZIGBEE_CS_PORT GPIOA
#define GPIO_ZIGBEE_CS_PIN GPIO_PIN_15
#define GPIO_ZIGBEE_EN_PORT GPIOB
#define GPIO_ZIGBEE_EN_PIN GPIO_PIN_13
//esp reset pin
#define GPIO_ESP_EN_PORT GPIOB
#define GPIO_ESP_EN_PIN GPIO_PIN_12

//adc zone
#define GPIO_ZONE_ADC_PORT GPIOA
#define GPIO_ZONE_ADC_PIN GPIO_PIN_6

//nrf ping
#define GPIO_NRF_PING_PORT GPIOA
#define GPIO_NRF_PING_PIN GPIO_PIN_8

void board_hw_initialize(void);
void board_adc_init(void);
uint32_t board_hw_read_adc_ntc (void);
uint32_t get_temperature (void);

#endif /*BOARD_HW_H */
