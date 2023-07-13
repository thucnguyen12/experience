#ifndef __GD32_I2C_H
#define __GD32_I2C_H

#include "main.h"
#include "gd32f3x0.h"
#include <stdint.h>
#include "i2c_gd_base.h"
#include "gd32f3x0_rcu.h"
#define I2CX_Pin_AFx_BIT(gpiox,pinx,afx) (((uint32_t)gpiox)|((uint32_t)(pinx)<<4)|(uint32_t)(afx))
typedef enum
{
    Send_7Address = 0x42,
    Send_7Address_2 = 0x44,
    Send_lcd_Address = 0x41,
    send_gual = 0x00,
    Send_10Address = 0x232
}Send_Address_Enum;

typedef enum
{
    standard_speed = 100000,
    High_speed = 400000,
    High_speed_plus = 1000000
}I2C_Speed_enum;

typedef enum 
{
    I2C0_SDA_GPIOx1= I2CX_Pin_AFx_BIT(GPIOA,10,4),
    I2C0_SDA_GPIOx2= I2CX_Pin_AFx_BIT(GPIOB,7,1),
    I2C0_SDA_GPIOx3= I2CX_Pin_AFx_BIT(GPIOB,9,1)
} I2C0_PinSDA_Enum;

typedef enum 
{
    I2C0_SCL_GPIOy1= I2CX_Pin_AFx_BIT(GPIOA,9,4),
    I2C0_SCL_GPIOy2= I2CX_Pin_AFx_BIT(GPIOB,6,1),
    I2C0_SCL_GPIOy3= I2CX_Pin_AFx_BIT(GPIOB,8,1)
} I2C0_PinSCL_Enum;

typedef enum
{
    I2Cx_Adress_7bit= I2C_ADDFORMAT_7BITS,
    I2Cx_Adress_10bit= I2C_ADDFORMAT_10BITS
}Adress_Model_Enum;

typedef enum
{
    Slave_7Address =0x40,
    Slave_lcd = 0x41,
    Slave_10Address = 0x230
}Slave_Address_Enum;

typedef struct
{
    uint32_t Memory_address;
    I2C0_PinSDA_Enum sda_pin;
    I2C0_PinSCL_Enum scl_pin;
    rcu_periph_enum I2c_Periph;
    I2C_Speed_enum i2c_speed;
    Adress_Model_Enum address_model;
    Send_Address_Enum address;
    Slave_Address_Enum slave_address;
} i2c_para_typedef;

void I2Cx_Init(i2c_para_typedef* i2c_init);

void I2Cx_Pin_ClockEnable (I2C0_PinSDA_Enum sda_pin, I2C0_PinSCL_Enum scl_pin);

void Master_SendData_address (uint8_t* SendBuffer, uint8_t data_len, i2c_para_typedef* i2c_init);

void Master_ReceiveData (uint8_t* ReceiveBuffer, uint8_t data_len, i2c_para_typedef* i2c_init);

void Send_7Prepare (i2c_para_typedef* i2c_init);

void Send_10Prepare (i2c_para_typedef* i2c_init);

void Receive_7Prepare (i2c_para_typedef* i2c_init);

void Receive_10Prepare (i2c_para_typedef* i2c_init);
int pt2257_i2c_tx(uint8_t *data, uint32_t size);

#endif