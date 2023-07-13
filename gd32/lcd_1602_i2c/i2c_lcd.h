#ifndef I2C_LCD_H_
#define I2C_LCD_H_

#include <stdint.h>

#define I2C_LCD_ADDR 0x4E

#define I2C_LCD_Delay_Ms(u16DelayMs) Delay_Ms(u16DelayMs)

#define SLAVE_ADDRESS_LCD 0x4E
#define HAL_Delay(TIME) 			ets_delay_us(TIME*100);


#define SLAVE_ADDRESS_LCD 0x4E // change this according to ur setup

#define LCD_EN 2
#define LCD_RW 1
#define LCD_RS 0
#define LCD_D4 4
#define LCD_D5 5
#define LCD_D6 6
#define LCD_D7 7
#define LCD_BL 3

void I2C_LCD_Init(void);
void I2C_LCD_Puts(char *szStr);
void I2C_LCD_Clear(void);
void I2C_LCD_NewLine(void);
void I2C_LCD_BackLight(uint8_t u8BackLight);

void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_clear_display (void);	//clear display lcd

void lcd_goto_XY (int row, int col); //set proper location on screen

#endif
