
#include "i2c_lcd.h"
#include "i2c_gd_base.h"
#include "board_hw.h"
#include "main.h"
#include "gd32f3x0.h"
#include "app_debug.h"
#include "gd32_i2c.h"

static uint8_t u8LCD_Buff[8];//bo nho dem luu lai toan bo
static uint8_t u8LcdTmp;

#define	MODE_4_BIT		0x28
#define	CLR_SCR			0x01
#define	DISP_ON			0x0C
#define	CURSOR_ON		0x0E
#define	CURSOR_HOME		0x80

uint8_t data[4];
#define LCD_I2C_TRANSMIT(ADD,DATA,SIZE0)   i2c_master_write_slave(I2C0, DATA, SIZE0, ADD);

static void I2C_LCD_Write_4bit(uint8_t u8Data);
static void I2C_LCD_FlushVal(void);
static void I2C_LCD_WriteCmd(uint8_t u8Cmd);

//void I2C_LCD_FlushVal(void)
//{
//	uint8_t i;
//	
//	for (i = 0; i < 8; ++i) {
//		u8LcdTmp >>= 1;
//		if(u8LCD_Buff[i]) {
//			u8LcdTmp |= 0x80;
//		}
//	}
//	I2C_Write(I2C_LCD_ADDR, &u8LcdTmp, 1);
//}

//void I2C_LCD_Init(void)
//{
//	uint8_t i;
////	
////	//I2C_LCD_Delay_Ms(50);
////	
////	My_I2C_Init();
//	
//	for (i = 0; i < 8; ++i) {
//		u8LCD_Buff[i] = 0;
//	}
//	
//	I2C_LCD_FlushVal();
//	
//	u8LCD_Buff[LCD_RS] = 0;
//	I2C_LCD_FlushVal();
//	
//	u8LCD_Buff[LCD_RW] = 0;
//	I2C_LCD_FlushVal();
//	
//	I2C_LCD_Write_4bit(0x03);
////	//I2C_LCD_Delay_Ms(5);
//	sys_delay_ms(5);
//    
//	I2C_LCD_Write_4bit(0x03);
////	//I2C_LCD_Delay_Ms(1);
//	sys_delay_ms(1);
//	I2C_LCD_Write_4bit(0x03);
//	//I2C_LCD_Delay_Ms(1);
//    sys_delay_ms(1);
//	I2C_LCD_Write_4bit(MODE_4_BIT >> 4);
//	//I2C_LCD_Delay_Ms(1);
//    sys_delay_ms(1);
//	I2C_LCD_WriteCmd(MODE_4_BIT);
//	I2C_LCD_WriteCmd(DISP_ON);
//	I2C_LCD_WriteCmd(CURSOR_ON);
//	I2C_LCD_WriteCmd(CLR_SCR);
//}

//void I2C_LCD_Write_4bit(uint8_t u8Data)
//{
//	//4 bit can ghi chinh la 4 5 6 7
//	//dau tien gan LCD_E=1
//	//ghi du lieu
//	//sau do gan LCD_E=0

//	if(u8Data & 0x08) {
//		u8LCD_Buff[LCD_D7] = 1;
//	} else {
//		u8LCD_Buff[LCD_D7] = 0;
//	}
//	if(u8Data & 0x04) {
//		u8LCD_Buff[LCD_D6] = 1;
//	} else {
//		u8LCD_Buff[LCD_D6] = 0;
//	}
//	if(u8Data & 0x02) {
//		u8LCD_Buff[LCD_D5] = 1;
//	} else {
//		u8LCD_Buff[LCD_D5] = 0;
//	}
//	if(u8Data & 0x01) {
//		u8LCD_Buff[LCD_D4] = 1;
//	} else {
//		u8LCD_Buff[LCD_D4] = 0;
//	}
//	
//	u8LCD_Buff[LCD_EN] = 1;
//	I2C_LCD_FlushVal();	
//	
//	u8LCD_Buff[LCD_EN] = 0;
//	I2C_LCD_FlushVal();
//	
//}

//void LCD_WaitBusy(void)
//{
//	uint8_t temp;
//	
//	//dau tien ghi tat ca 4 bit thap bang 1
//	u8LCD_Buff[LCD_D4] = 1;
//	u8LCD_Buff[LCD_D5] = 1;
//	u8LCD_Buff[LCD_D6] = 1;
//	u8LCD_Buff[LCD_D7] = 1;
//	I2C_LCD_FlushVal();
//	
//	u8LCD_Buff[LCD_RS] = 0;
//	I2C_LCD_FlushVal();
//	
//	u8LCD_Buff[LCD_RW] = 1;
//	I2C_LCD_FlushVal();
//	
//	do {
//		u8LCD_Buff[LCD_EN] = 1;
//		I2C_LCD_FlushVal();
//		I2C_Read(I2C_LCD_ADDR + 1, &temp, 1);
//		
//		u8LCD_Buff[LCD_EN] = 0;
//		I2C_LCD_FlushVal();
//		u8LCD_Buff[LCD_EN] = 1;
//		I2C_LCD_FlushVal();
//		u8LCD_Buff[LCD_EN] = 0;
//		I2C_LCD_FlushVal();
//	} while (temp & 0x08);
//}

//void I2C_LCD_WriteCmd(uint8_t u8Cmd)
//{
//	
//	LCD_WaitBusy();

//	u8LCD_Buff[LCD_RS] = 0;
//	I2C_LCD_FlushVal();
//	
//	u8LCD_Buff[LCD_RW] = 0;
//	I2C_LCD_FlushVal();
//	
//	I2C_LCD_Write_4bit(u8Cmd >> 4);
//	I2C_LCD_Write_4bit(u8Cmd);
//}

//void LCD_Write_Chr(char chr)
//{
//	
//	LCD_WaitBusy();
//	u8LCD_Buff[LCD_RS] = 1;
//	I2C_LCD_FlushVal();
//	u8LCD_Buff[LCD_RW] = 0;
//	I2C_LCD_FlushVal();
//	I2C_LCD_Write_4bit(chr >> 4);
//	I2C_LCD_Write_4bit(chr);
//	
//}

//void I2C_LCD_Puts(char *sz)
//{
//	
//	while (1) {
//		if (*sz) {
//			LCD_Write_Chr(*sz++);
//		} else {
//			break;
//		}
//	}
//}

//void I2C_LCD_Clear(void)
//{
//	
//	I2C_LCD_WriteCmd(CLR_SCR);
//}

//void I2C_LCD_NewLine(void)
//{
//	
//	I2C_LCD_WriteCmd(0xc0);
//}

//void I2C_LCD_BackLight(uint8_t u8BackLight)
//{
//	
//	if(u8BackLight) {
//		u8LCD_Buff[LCD_BL] = 1;
//	} else {
//		u8LCD_Buff[LCD_BL] = 0;
//	}
//	I2C_LCD_FlushVal();
//}

static i2c_para_typedef I2Cx_Param =
{
    I2C0, I2C0_SDA_GPIOx3, I2C0_SCL_GPIOy3, RCU_I2C0, standard_speed, I2Cx_Adress_7bit, Send_lcd_Address, Slave_lcd
};


void lcd_send_cmd (char cmd)
{
  char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd&0xf0);
	data_l = ((cmd<<4)&0xf0);
	data_t[0] = data_u|0x0C;  //en=1, rs=0
	data_t[1] = data_u|0x08;  //en=0, rs=0
	data_t[2] = data_l|0x0C;  //en=1, rs=0
	data_t[3] = data_l|0x08;  //en=0, rs=0
	//HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
//	LCD_I2C_TRANSMIT(SLAVE_ADDRESS_LCD,data_t,4);
    Master_SendData_address (data_t, 4, &I2Cx_Param);
}

void lcd_send_data (char data)
{
    DEBUG_INFO ("DATA  %c", data);
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data&0xf0);
	data_l = ((data<<4)&0xf0);
	data_t[0] = data_u|0x0D;  //en=1, rs=0
	data_t[1] = data_u|0x09;  //en=0, rs=0
	data_t[2] = data_l|0x0D;  //en=1, rs=0
	data_t[3] = data_l|0x09;  //en=0, rs=0
	//HAL_I2C_Master_Transmit (&hi2c1, SLAVE_ADDRESS_LCD,(uint8_t *) data_t, 4, 100);
//	LCD_I2C_TRANSMIT(SLAVE_ADDRESS_LCD,data_t,4);
//    DEBUG_INFO ("DATA SEND %c\r\n", data_t);
    
    Master_SendData_address (data_t, 4, &I2Cx_Param);
}

void lcd_init (void)
{
//	i2c_config();
    I2Cx_Init (&I2Cx_Param);
    sys_delay_ms(3000);
    DEBUG_INFO ("I2C CONFIG\r\n");
	lcd_send_cmd (0x33); /* set 4-bits interface */
	lcd_send_cmd (0x32);
	sys_delay_ms(100);
	lcd_send_cmd (0x28); /* start to set LCD function */
	sys_delay_ms(100);
	lcd_send_cmd (0x01); /* clear display */
	sys_delay_ms(100);
	lcd_send_cmd (0x06); /* set entry mode */
	sys_delay_ms(100);
	lcd_send_cmd (0x0c); /* set display to on */	
	sys_delay_ms(100);
	lcd_send_cmd (0x02); /* move cursor to home and set data address to 0 */
	sys_delay_ms(100);
	lcd_send_cmd (0x80);
}

void lcd_send_string (char *str)
{
    DEBUG_INFO ("STRING: %s", str);
	while (*str) {
        lcd_send_data (*str++);
        sys_delay_ms(2);
    }        
}

void lcd_clear_display (void)
{
	lcd_send_cmd (0x01); //clear display
    sys_delay_ms(1000);
}

void lcd_goto_XY (int row, int col)
{
	uint8_t pos_Addr;
	if(row == 1) 
	{
		pos_Addr = 0x80 + row - 1 + col;
	}
	else
	{
		pos_Addr = 0x80 | (0x40 + col);
	}
	lcd_send_cmd(pos_Addr);
}

