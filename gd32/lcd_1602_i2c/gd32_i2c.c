#include "gd32_i2c.h"
#include "app_debug.h"
#include "board_hw.h"
#define BufferNum 20

uint8_t write_buffer [BufferNum] = {0};
uint8_t read_buffer [BufferNum] = {0};



static i2c_para_typedef para_array[]=
{
    {I2C0, I2C0_SDA_GPIOx3, I2C0_SCL_GPIOy3, RCU_I2C0, standard_speed, I2Cx_Adress_7bit, Send_7Address, Slave_7Address}
};


void I2Cx_Init(i2c_para_typedef* i2c_init)
{
//    uint32_t gpio_sda, pin_sda, afx_sda, gpio_scl, pin_scl, afx_scl;
//    I2Cx_Pin_ClockEnable (i2c_init->sda_pin, i2c_init->scl_pin);
//    gpio_sda = (uint32_t) ((i2c_init->sda_pin)&0xffffff00);
//    pin_sda= (0x01<<((i2c_init->sda_pin & 0x0f) >> 4));
//    afx_sda = AF((i2c_init->sda_pin & 0x0f));
//    
//    I2Cx_Pin_ClockEnable (i2c_init->sda_pin, i2c_init->scl_pin);
//    gpio_af_set (gpio_sda, afx_sda, pin_sda);
//    gpio_mode_set (gpio_sda, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin_sda);
//    gpio_output_options_set(gpio_sda, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, pin_sda);
//    
//    I2Cx_Pin_ClockEnable (i2c_init->sda_pin, i2c_init->scl_pin);
//    gpio_scl = (uint32_t) ((i2c_init->scl_pin)&0xffffff00);
//    pin_scl= (0x01<<((i2c_init->scl_pin & 0x0f) >> 4));
//    afx_scl = AF((i2c_init->scl_pin & 0x0f));
//    
//    gpio_af_set (gpio_scl, afx_scl, pin_scl);
//    gpio_mode_set (gpio_scl, GPIO_MODE_AF, GPIO_PUPD_PULLUP, pin_scl);
//    gpio_output_options_set(gpio_scl, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, pin_scl);
//    
//    rcu_periph_clock_enable(i2c_init->I2c_Periph);
//    
//    i2c_clock_config(i2c_init->Memory_address, i2c_init->i2c_speed, I2C_DTCY_2);
//    /* I2C address configure */
//    i2c_mode_addr_config(i2c_init->Memory_address, I2C_I2CMODE_ENABLE, i2c_init->address_model, i2c_init->slave_address);
//    /* enable I2C0 */
//    i2c_enable(i2c_init->Memory_address);
//    /* enable acknowledge */
//    i2c_ack_config(i2c_init->Memory_address, I2C_ACK_ENABLE);
    
    
        /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable KT0935_I2C clock */
    rcu_periph_clock_enable(RCU_I2C0);
    /* connect PB8 to I2C0_SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_8);
    /* connect PB7 to I2C0_SDA */
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_9);
    /* configure GPIO pins of KT0935_I2C */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_8);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_8);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ, GPIO_PIN_9);

    /* I2C clock configure */
    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
    /* I2C address configure */
    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, 0x40);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}

void I2Cx_Pin_ClockEnable (I2C0_PinSDA_Enum sda_pin, I2C0_PinSCL_Enum scl_pin)
{
    uint32_t gpio_sda, gpio_scl;
    gpio_sda = (uint32_t)(sda_pin & 0xffffff00);
    gpio_scl = (uint32_t)(scl_pin & 0xffffff00);
    
    if (gpio_sda == GPIOB)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
    }
    else if (gpio_sda == GPIOA)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
    }
    else if (gpio_sda == GPIOF)
    {
        rcu_periph_clock_enable(RCU_GPIOF);
    }
    
    
    if (scl_pin == GPIOB)
    {
        rcu_periph_clock_enable(RCU_GPIOB);
    }
    else if (scl_pin == GPIOA)
    {
        rcu_periph_clock_enable(RCU_GPIOA);
    }
}

void Master_SendData_address (uint8_t* SendBuffer, uint8_t data_len, i2c_para_typedef* i2c_init)
{
    uint8_t num;
    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_I2CBSY)){
    }
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    if (i2c_init->address_model == I2Cx_Adress_7bit)
    {
        Send_7Prepare (i2c_init);
    }
    else if (i2c_init->address_model == I2Cx_Adress_10bit)
    {
        Send_10Prepare (i2c_init);
    }
//    DEBUG_INFO ("I2C data:");
    for (uint8_t i = 0; i < data_len; i++)
    {
        i2c_data_transmit(I2C0, *(SendBuffer+i));
//        DEBUG_INFO ("%02x", *(SendBuffer+i));
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
//        if ((i%2)==0)
//        {
//            sys_delay_ms(5);
//        }

    }
    DEBUG_INFO ("\r\n");
    /* send a stop condition to I2C bus*/
    i2c_stop_on_bus(I2C0);
    /* wait until stop condition generate */
    while(I2C_CTL0(I2C0)&0x0200);
}

void Master_ReceiveData (uint8_t* ReceiveBuffer, uint8_t data_len, i2c_para_typedef* i2c_init)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_I2CBSY));
    /* send a start condition to I2C bus */
    i2c_start_on_bus(i2c_init->Memory_address);
    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_SBSEND));
    if ((i2c_init->address == Send_7Address) || (i2c_init->address == Send_lcd_Address))
    {
        Receive_7Prepare (i2c_init);
    }
    else if (i2c_init->address == Send_10Address)
    {
        Receive_10Prepare (i2c_init);
    }
    if (data_len == 1)
    {
        i2c_ack_config (i2c_init->Memory_address, I2C_ACK_DISABLE);
        i2c_stop_on_bus (i2c_init->Memory_address);
        
        while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_RBNE));
        *(ReceiveBuffer) = i2c_data_receive(i2c_init->Memory_address);
    }
    while (data_len)
    {
        if (data_len == 2)
        {
            while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_RBNE));
            *(ReceiveBuffer) = i2c_data_receive(i2c_init->Memory_address);
            data_len-=1;
            ReceiveBuffer++;
            i2c_ack_config (i2c_init->Memory_address, I2C_ACK_DISABLE);
            i2c_stop_on_bus (i2c_init->Memory_address);
        }
        while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_RBNE));
        *(ReceiveBuffer) = i2c_data_receive(i2c_init->Memory_address);
        data_len-=1;
        ReceiveBuffer++;
    }
    /* wait until stop condition generate */
    while(I2C_CTL0(i2c_init->Memory_address)&0x0200);
    /* enable acknowledge */
    i2c_ack_config(i2c_init->Memory_address, I2C_ACK_ENABLE);
    i2c_ackpos_config (i2c_init->Memory_address, I2C_ACKPOS_CURRENT);
}
void Send_7Prepare (i2c_para_typedef* i2c_init)
{
    
    i2c_master_addressing(i2c_init->Memory_address, i2c_init->address, I2C_TRANSMITTER);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADDSEND));
    i2c_flag_clear(i2c_init->Memory_address, I2C_FLAG_ADDSEND);
    
}

void Send_10Prepare (i2c_para_typedef* i2c_init)
{
    uint32_t adrH10, adrL10;
    adrH10 = (((i2c_init->address & 0x300) >> 7) | 0xf0);
    adrL10 = i2c_init->address & 0xff;
    i2c_master_addressing(i2c_init->Memory_address, (uint8_t) adrH10, I2C_TRANSMITTER);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADD10SEND));
    i2c_data_transmit (i2c_init->Memory_address, adrL10);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
}

void Receive_7Prepare (i2c_para_typedef* i2c_init)
{
    i2c_master_addressing(i2c_init->Memory_address, i2c_init->address, I2C_RECEIVER);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
}

void Receive_10Prepare (i2c_para_typedef* i2c_init)
{
    uint32_t adrH10, adrL10;
    adrH10 = (((i2c_init->address & 0x300) >> 7) | 0xf0);
    adrL10 = i2c_init->address & 0xff;
    i2c_master_addressing(i2c_init->Memory_address, (uint8_t) adrH10, I2C_TRANSMITTER);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADD10SEND));
    i2c_data_transmit (i2c_init->Memory_address, adrL10);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    i2c_start_on_bus(I2C0);
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));
    i2c_master_addressing(i2c_init->Memory_address, i2c_init->address, I2C_RECEIVER);
    while(!i2c_flag_get(i2c_init->Memory_address, I2C_FLAG_ADDSEND));
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
}

int pt2257_i2c_tx(uint8_t *data, uint32_t size)
{
//    HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hI2C0, (PT2257_ADDR), data, size, 10);
//    if (status != HAL_OK)
//    {
//        DEBUG_ERROR("I2C TX failed %d\r\n", status);
//        return -1;
//    }
//    return 0;
    
    #define I2C_TIMEOUT ((uint32_t)5)
    
    uint32_t i2c_start_ms = sys_get_ms();
    bool i2c_timeout_err = false;        // 1ms
    bool retval = false;
//    uint32_t tmp;

    
    /* wait until I2C bus is idle */
    while (i2c_flag_get(I2C0, I2C_FLAG_I2CBSY))
    {
        if (sys_get_ms() - i2c_start_ms >= 3*I2C_TIMEOUT)
        {
            DEBUG_ERROR("I2C busy\r\n");
            i2c_timeout_err = true; 
            break;
        }
    }
    if (i2c_timeout_err)
    {
        goto end;
    }        
    
    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);
    
    /* wait until SBSEND bit is set */
    i2c_start_ms = sys_get_ms();
    while (!i2c_flag_get(I2C0, I2C_FLAG_SBSEND))
    {
        if (sys_get_ms() - i2c_start_ms >= I2C_TIMEOUT)
        {
            i2c_timeout_err = true; 
			break;
        }
    }
    if (i2c_timeout_err)
    {
        goto end;
    } 
    
    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, 0x41, I2C_TRANSMITTER);
    i2c_start_ms = sys_get_ms();

    
//    /* disable ACK before clearing ADDSEND bit */
//    i2c_ack_config(I2C0, I2C_ACK_DISABLE);
    
    
    /* wait until ADDSEND bit is set */
    i2c_start_ms = sys_get_ms();
    while (!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND))
    {
        if (sys_get_ms() - i2c_start_ms >= I2C_TIMEOUT)
        {
            i2c_timeout_err = true; 
            break;
        }
    }
    
    if (i2c_timeout_err)
    {
        goto end;
    }     
     
    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);
    
    for (uint32_t i = 0; i < size; i++)       // 32 = 4 bytes adc * 8zone
    {
        i2c_start_ms = sys_get_ms();
        
        if (!i2c_timeout_err)
        {
            /* wait until the RBNE bit is set */
            i2c_data_transmit(I2C0, data[i]);
            
            while (!i2c_flag_get(I2C0, I2C_FLAG_TBE))
            {
                if (sys_get_ms() - i2c_start_ms >= I2C_TIMEOUT)
                {
                    i2c_timeout_err = true; 
                    break;
                }
            }
        }

        if (i2c_timeout_err)
        {
            break;
        }
    }
    
    /* send a stop condition */
    i2c_stop_on_bus(I2C0);
    if (!i2c_timeout_err)
    {
        i2c_start_ms = sys_get_ms();
        while (I2C_CTL0(I2C0)&0x0200)
        {
            if (sys_get_ms() - i2c_start_ms >= I2C_TIMEOUT)
            {
                i2c_timeout_err = true; 
                break;
            }
        }
    }
//    i2c_ackpos_config(I2C0, I2C_ACKPOS_CURRENT);
    /* enable acknowledge */
        
    
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
    
    if (!i2c_timeout_err)
    {
        retval = true;
    }
end:
    i2c_stop_on_bus(I2C0);
    if (!retval)
    {   
        DEBUG_ERROR("I2C failed\r\n");
        return -1;
    }
    return 0;
}
