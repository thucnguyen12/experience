/*!
    \file    main.c
    \brief   led spark with systick, USART print and key example
    
    \version 2019-02-19, V1.0.0, firmware for GD32E23x
    \version 2020-12-12, V1.1.0, firmware for GD32E23x
*/

/*
    Copyright (c) 2020, GigaDevice Semiconductor Inc.

    Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this 
       list of conditions and the following disclaimer.
    2. Redistributions in binary form must reproduce the above copyright notice, 
       this list of conditions and the following disclaimer in the documentation 
       and/or other materials provided with the distribution.
    3. Neither the name of the copyright holder nor the names of its contributors 
       may be used to endorse or promote products derived from this software without 
       specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY 
OF SUCH DAMAGE.
*/

#include "gd32f3x0.h"
#include "systick.h"
#include <stdio.h>
#include "main.h"
#include "board_hw/board_hw.h"
#include "lwrb/include/lwrb/lwrb.h"
#include "min_protocol/min.h"
#include "min_protocol/min_id.h"

#include "flash.h"
#include "gd32f3x0_it.h"
#include "u8g2.h"
#include "app_debug.h"
#include "SEGGER_RTT.h"
#include "SEGGER_RTT_Conf.h"
#include "i2c_lcd.h"
#define THIS_IS_TEST_FIRMWARE 1

#define RTC_CLOCK_SOURCE_IRC40K
#define BKP_VALUE    0x32F0
//time defination
#define FIRSTYEAR 2000 // start year
#define FIRSTDAY 6	   // 0 = Sunday

#define DELAY_ALARM_TIME 30
//volatile uint32_t m_delay_remain = 0;

typedef struct
{
	uint8_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	uint8_t second;
	uint8_t weekday;
} date_time_t;

static const uint8_t day_in_month[12] = {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

uint32_t confirm_alarm_time_in_second;

lwrb_t uart_rb;
uint8_t uart_buffer[256];
//uint8_t min_txbuffer[1024];

uint8_t min_rx_buffer[256];
min_context_t m_min_context;
min_frame_cfg_t m_min_setting;


// LCD RELATE VAR
typedef struct __attribute((packed))
{
    char MAC[6];
    uint8_t  deviceType;
    date_time_t time_got_trouble;
    bool is_there_data;
} trouble_info_of_note_t;

static char m_max_zone_error_buffer[48][20];

date_time_t first_time_got_trouble;
bool there_are_trouble = false;
bool last_trouble_state = false;
// LCD VAR end

// esp32 info var

bool check_ping_esp_s = false;

rtc_parameter_struct rtc_time;
//rtc_tamper_struct rtc_tamper;
rtc_parameter_struct rtc_initpara;
__IO uint32_t prescaler_a = 0, prescaler_s = 0;


// spi var
//volatile uint32_t send_n = 0, receive_n = 0;
//uint8_t spi0_send_array[ARRAYSIZE];
//uint8_t spi0_receive_array[ARRAYSIZE];
//uint8_t spi_data_flag = 0;
volatile uint32_t sys_counter;


//PROTOTYPE PLACE
void irc40k_config(void);
static uint8_t convert_input_to_bcd_format(uint8_t value);
uint8_t convert_bcd_to_dec (uint8_t bcd);
void rtc_setup(date_time_t *t);
void rtc_pre_config(void);
void rtc_show_timestamp(void);
void update_time(uint32_t timestamp);
uint32_t convert_date_time_to_second(date_time_t *t);

uint8_t get_weekday(date_time_t time);

void convert_second_to_date_time(uint32_t sec, date_time_t *t, uint8_t Calyear);

void lcd_display_trouble (trouble_info_of_note_t* trouble_info_table, uint8_t trouble_sensor_cnt);

void convert_rtc_time_to_date_time_t (date_time_t* date_time);


//PROTOTYPE PLACE END    

//genaeral var
bool in_pair_mode = false;
static bool got_mac = false;
static bool got_info_of_node_to_pair = false;
static uint8_t sensor_trouble_count = 0;
static uint32_t counter = 0;
//
/*!
    \brief      configure the SPI peripheral
    \param[in]  none
    \param[out] none
    \retval     none
*/
void spi_config(void)
{
//    gpio_bit_set (GPIO_NRF_CS_PORT, GPIO_NRF_CS_PIN);
//    spi_parameter_struct spi_init_struct;
//    
//    /* deinitilize SPI and the parameters */
//    spi_i2s_deinit(SPI0);
////    spi_i2s_deinit(SPI1);
//    rcu_periph_clock_enable(RCU_SPI0);
//    spi_struct_para_init(&spi_init_struct);

//    /* configure SPI0 parameter */
//    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
//    spi_init_struct.device_mode          = SPI_MASTER;
//    spi_init_struct.frame_size           = SPI_FRAMESIZE_8BIT;
//    spi_init_struct.clock_polarity_phase = SPI_CK_PL_LOW_PH_1EDGE;
//    spi_init_struct.nss                  = SPI_NSS_SOFT;
//    spi_init_struct.prescale             = SPI_PSC_64;
//    spi_init_struct.endian               = SPI_ENDIAN_MSB;
//    spi_init(SPI0, &spi_init_struct);

//    /* configure SPI1 parameter */
////    spi_init_struct.trans_mode           = SPI_TRANSMODE_FULLDUPLEX;
////    spi_init_struct.device_mode          = SPI_SLAVE;
////    spi_init(SPI1, &spi_init_struct);

//    /* configure SPI1 byte access to FIFO */
////    spi_fifo_access_size_config(SPI1, SPI_BYTE_ACCESS);

#if SPI_CRC_ENABLE
    /* configure SPI CRC function */
    spi_crc_length_set(SPI1, SPI_CRC_8BIT);
    spi_crc_polynomial_set(SPI0, 7);
    spi_crc_polynomial_set(SPI1, 7);
    spi_crc_on(SPI0);
    spi_crc_on(SPI1);
#endif /* enable CRC function */
}


//get tick
uint32_t sys_get_ms(void)
{
    return sys_counter;
}

//Handle min data after receive
//uint8_t spi_data_buffer[251];

uint16_t unicast_addr;

uint8_t spi_retry_cnt;
uint8_t gpio_logic = 1;
bool first_time_update = true;
uint8_t gsm_csq_now = 0;
void min_rx_callback(void *min_context, min_msg_t *frame)
{
    DEBUG_WARN ("MIN ID: %d\r\n", frame->id);
    switch (frame->id)
    {
    case MIN_ID_PING_ESP_ALIVE:

    default:
//        DEBUG_ERROR ("WRONG ID : %02x", frame->id);
        break;
    }
}

bool min_tx_byte(void *ctx, uint8_t byte)
{
	(void)ctx;
	usart_data_transmit(USART0, (uint8_t)byte);
    while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
	return true;
}

void send_min_data(min_msg_t *min_msg)
{
	min_send_frame(&m_min_context, min_msg);
}


//LCD PLACE function


uint32_t rtt_tx(const void *buffer, uint32_t size)
{
    return SEGGER_RTT_Write(0, buffer, size);
}

/*!
    \brief      main function
    \param[in]  none
    \param[out] none
    \retval     none
*/
void irc40k_config(void)
{
    /* enable IRC40K */
    rcu_osci_on(RCU_IRC40K);
    /* wait till IRC40K is ready */
    rcu_osci_stab_wait(RCU_IRC40K);
}

void rtc_pre_config(void)
{
    #if defined (RTC_CLOCK_SOURCE_IRC40K) 
          //rcu_osci_on(RCU_IRC40K); //use same clock source with watchdog so don't need turn it on again
          //rcu_osci_stab_wait(RCU_IRC40K);
          rcu_rtc_clock_config (RCU_RTCSRC_IRC40K);
  
          prescaler_s = 0x18F;
          prescaler_a = 0x63;
    #elif defined (RTC_CLOCK_SOURCE_LXTAL)
          rcu_osci_on(RCU_LXTAL);
          rcu_osci_stab_wait(RCU_LXTAL);
          rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
          prescaler_s = 0xFF;
          prescaler_a = 0x7F;
    #else
    #error RTC clock source should be defined.
    #endif /* RTC_CLOCK_SOURCE_IRC40K */

    rcu_periph_clock_enable (RCU_RTC);
    rtc_register_sync_wait();
}

static uint8_t convert_input_to_bcd_format(uint8_t value)
{
    uint32_t index = 0;
    uint32_t tmp[2] = {0, 0};
    if (value < 10)
    {
        tmp[0] = 0;
        tmp[1] = value;
    }
    else if ((value >= 10) && (value < 100))
    {
        tmp[0] = value / 10;
        tmp[1] = value % 10;
    }
    else
    {
        DEBUG_ERROR ("INPUT TIME VALUE WRONG: %d", value);
    }
    index = (tmp[1]) + ((tmp[0]) <<4);
    return index;
}
uint8_t convert_bcd_to_dec (uint8_t bcd)
{
   uint32_t index = 0;
   uint32_t tmp[2] = {0, 0};
   if (bcd < 0x10)
   {
       tmp[0] = 0;
       tmp[1] = bcd;
   }
   else
   {
       tmp[1] = bcd & 0x0f;
       tmp[0] = (bcd & 0xf0) >> 4;
   }
   index = (tmp[1]) + ((tmp[0])*10);
   return index;
}


void rtc_setup(date_time_t *t)
{
    /* setup RTC time value */
    uint32_t tmp_hh = 0xFF, tmp_mm = 0xFF, tmp_ss = 0xFF;

    rtc_initpara.rtc_factor_asyn = prescaler_a;
    rtc_initpara.rtc_factor_syn = prescaler_s;
    rtc_initpara.rtc_display_format = RTC_24HOUR;
    rtc_initpara.rtc_am_pm = RTC_AM;
    if (t == NULL)
    {
        rtc_initpara.rtc_year = 0x16;
        rtc_initpara.rtc_day_of_week = RTC_SATURDAY;
        rtc_initpara.rtc_month = RTC_APR;
        rtc_initpara.rtc_date = 0x30;
        rtc_initpara.rtc_hour = 0;
        rtc_initpara.rtc_minute = 0;
        rtc_initpara.rtc_second = 0;
    }
    else
    {
        //DEBUG_INFO ("init time now");
        rtc_initpara.rtc_year = convert_input_to_bcd_format (t->year);
        rtc_initpara.rtc_month = (t->month);
        rtc_initpara.rtc_date = convert_input_to_bcd_format (t->day);
        if (get_weekday (*t) == 0)
        {
           rtc_initpara.rtc_day_of_week = RTC_SUNDAY; 
        }
        else
        {
           rtc_initpara.rtc_day_of_week =  get_weekday (*t);
        }
        rtc_initpara.rtc_hour =(t->hour);
        rtc_initpara.rtc_minute = convert_input_to_bcd_format (t->minute);
        rtc_initpara.rtc_second = convert_input_to_bcd_format (t->second);
//        DEBUG_INFO ("TIME INIT VALUE:\r\n");
//        DEBUG_INFO ("year: %0.2x\r\n", rtc_initpara.rtc_year);
//        DEBUG_INFO ("month: %0.2x\r\n", rtc_initpara.rtc_month);
//        DEBUG_INFO ("date: %0.2x\r\n", rtc_initpara.rtc_date);
//        DEBUG_INFO ("rtc_hour: %0.2x\r\n", rtc_initpara.rtc_hour);
//        DEBUG_INFO ("rtc_minute: %0.2x\r\n", rtc_initpara.rtc_minute);
//        DEBUG_INFO ("rtc_second: %0.2x\r\n", rtc_initpara.rtc_second);
    }
    
    /* RTC current time configuration */
    if(ERROR == rtc_init(&rtc_initpara)){    
        DEBUG_INFO("** RTC time configuration failed! **\n\r");
    }else{
        //DEBUG_INFO("** RTC time configuration success! **\n\r");
//        rtc_show_timestamp();
        RTC_BKP0 = BKP_VALUE;
    }   
}


void convert_rtc_time_to_date_time_t (date_time_t* date_time)
{
    rtc_current_time_get(&rtc_time);
    date_time->year = rtc_time.rtc_year;
    date_time->month = rtc_time.rtc_month;
    date_time->day = rtc_time.rtc_date;
    date_time->hour = rtc_time.rtc_hour;
    date_time->minute = rtc_time.rtc_minute;
    date_time->second = rtc_time.rtc_second;
    date_time->year = convert_bcd_to_dec(date_time->year);
    date_time->day = convert_bcd_to_dec(date_time->day);
    date_time->minute = convert_bcd_to_dec(date_time->minute);
    date_time->second = convert_bcd_to_dec(date_time->second);
}
//end time


uint16_t uart_data_leng;
static uint8_t device_count = 0;

uint32_t now = 0;
static uint32_t last_time = 0;
static uint32_t last_time_ping = 0;
static uint32_t last_time_ask_beacon_data = 0;
uint8_t databuff[128];

min_msg_t min_power_msg;

static uint32_t last_time_send_zone_state = 0;

uint8_t relay_state = 0;
uint8_t led_alram_state = 0;

/*!
    \brief      cofigure the I2C0 and I2C1 interfaces
    \param[in]  none
    \param[out] none
    \retval     none
*/
//void i2c_config(void)
//{
//    /* I2C clock configure */
//    i2c_clock_config(I2C0, 100000, I2C_DTCY_2);
//    /* I2C address configure */
//    i2c_mode_addr_config(I2C0, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, SLAVE_ADDRESS_LCD);
//    /* enable I2C0 */
//    i2c_enable(I2C0);
//    /* enable acknowledge */
//    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
//}

uint8_t i2c_transmitter[16];
void rcu_config(void);
void gpio_config(void);
void i2c_config(void);
int main(void)
{
    /* configure systick */
    systick_config();
    irc40k_config();
    board_hw_initialize();
    
//board_adc_init();
    
//    lwrb_init(&uart_rb, uart_buffer, sizeof(uart_buffer));
    
//    spi_config ();
//    spi_enable (SPI0);

//*********  CONFIG BACKUP VALUE AND RTC*******    
    /* enable access to RTC registers in backup domain */
    rcu_periph_clock_enable(RCU_PMU);
    pmu_backup_write_enable();
  
    rtc_pre_config();
   // rtc_tamper_disable(RTC_TAMPER0);

    /* check if RTC has aready been configured */
    if (BKP_VALUE != RTC_BKP0){    
        rtc_setup (NULL); 
        //DEBUG_INFO ("RTC FIRST CONFIG");
    }else{
        /* detect the reset source */
        if (RESET != rcu_flag_get(RCU_FLAG_PORRST)){
            DEBUG_INFO("power on reset occurred....\n\r");
        }else if (RESET != rcu_flag_get(RCU_FLAG_EPRST)){
            DEBUG_INFO("external reset occurred....\n\r");
        }
        //DEBUG_INFO("no need to configure RTC....\n\r");
               
        //rtc_show_time(); // WE NEED DISPLAY TIME ON LCD ONCE A SECOND
    }
    
    // app debug
    app_debug_init(sys_get_ms,NULL);
    app_debug_register_callback_print(rtt_tx);
    DEBUG_INFO ("APP DEBUG OK\r\n");
    //while(RESET == usart_flag_get(USART0, USART_FLAG_TC));
    usart_interrupt_enable(USART0, USART_INT_RBNE);
    //delay_1ms (500);
//    
//    fwdgt_config(1250, FWDGT_PSC_DIV32);
//    fwdgt_enable();



    lcd_init();
	lcd_clear_display();
    DEBUG_INFO ("lcd init OK\r\n");
//	I2C_LCD_BackLight(1);

    last_time = sys_get_ms ();
    gpio_bit_reset(BOARD_HW_CONTROL_LED_CTRL_PORT, BOARD_HW_CONTROL_LED_CTRL_PIN);
    static uint32_t last_tick = 0;
    uint32_t now = 0;
//    I2C_LCD_Clear();
    
//    I2C_LCD_NewLine();
    lcd_goto_XY(2,0);
    lcd_send_string("aBC");
    while (1)
    {
//        lcd_send_string("gd32vietthuong");
        now = sys_get_ms ();
        if (now - last_tick > 10000)
        {
//            lcd_goto_XY(1,1);

//            lcd_send_string("aBC");
            last_tick = now;
        }
        fwdgt_counter_reload();
    }
}
void rcu_config(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);
}

/*!
    \brief      cofigure the GPIO ports
    \param[in]  none
    \param[out] none
    \retval     none
*/
void gpio_config(void)
{
    /* connect PB6 to I2C0_SCL */
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_6);
    /* connect PB7 to I2C0_SDA */
    gpio_af_set(GPIOB, GPIO_AF_1, GPIO_PIN_7);
    /* configure GPIO pins of I2C0 */
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_6);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_6);
    gpio_mode_set(GPIOB, GPIO_MODE_AF, GPIO_PUPD_PULLUP,GPIO_PIN_7);
    gpio_output_options_set(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ,GPIO_PIN_7);
}

void convert_second_to_date_time(uint32_t sec, date_time_t *t, uint8_t Calyear)
{
	uint16_t day;
	uint8_t year;
	uint16_t days_of_year;
	uint8_t leap400;
	uint8_t month;

	t->second = sec % 60;
	sec /= 60;
	t->minute = sec % 60;
	sec /= 60;
	t->hour = sec % 24;

	if (Calyear == 0)
		return;

	day = (uint16_t)(sec / 24);

	year = FIRSTYEAR % 100;					   // 0..99
	leap400 = 4 - ((FIRSTYEAR - 1) / 100 & 3); // 4, 3, 2, 1

	for (;;)
	{
		days_of_year = 365;
		if ((year & 3) == 0)
		{
			days_of_year = 366; // leap year
			if (year == 0 || year == 100 || year == 200)
			{ // 100 year exception
				if (--leap400)
				{ // 400 year exception
					days_of_year = 365;
				}
			}
		}
		if (day < days_of_year)
		{
			break;
		}
		day -= days_of_year;
		year++; // 00..136 / 99..235
	}
	t->year = year + FIRSTYEAR / 100 * 100 - 2000; // + century
	if (days_of_year & 1 && day > 58)
	{		   // no leap year and after 28.2.
		day++; // skip 29.2.
	}

	for (month = 1; day >= day_in_month[month - 1]; month++)
	{
		day -= day_in_month[month - 1];
	}

	t->month = month; // 1..12
	t->day = day + 1; // 1..31
}

uint8_t get_weekday(date_time_t time)
{
	time.weekday = (time.day +=
					time.month < 3 ? time.year-- : time.year - 2,
					23 * time.month / 9 + time.day + 4 + time.year / 4 -
						time.year / 100 + time.year / 400);
	return time.weekday % 7;
}

uint32_t convert_date_time_to_second(date_time_t *t)
{
	uint8_t i;
	uint32_t result = 0;
	uint16_t idx, year;

	year = t->year + 2000;

	/* Calculate days of years before */
	result = (uint32_t)year * 365;
	if (t->year >= 1)
	{
		result += (year + 3) / 4;
		result -= (year - 1) / 100;
		result += (year - 1) / 400;
	}

	/* Start with 2000 a.d. */
	result -= 730485UL;

	/* Make month an array index */
	idx = t->month - 1;

	/* Loop thru each month, adding the days */
	for (i = 0; i < idx; i++)
	{
		result += day_in_month[i];
	}

	/* Leap year? adjust February */
	if (!(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0)))
	{
		if (t->month > 2)
		{
			result--;
		}
	}

	/* Add remaining days */
	result += t->day;

	/* Convert to seconds, add all the other stuff */
	result = (result - 1) * 86400L + (uint32_t)t->hour * 3600 +
			 (uint32_t)t->minute * 60 + t->second;
	return result;
}

void update_time(uint32_t timestamp)
{
    date_time_t time_now;
    timestamp -= 946684800;
    convert_second_to_date_time (timestamp, &time_now, 1);
    DEBUG_INFO ("TIME AFTER CONVERT: %d: %d: %d \r\n", time_now.hour, time_now.minute, time_now.second);
    rtc_setup (&time_now);
}
// RTC AND TIME PLACE END


void uart0_handler(void)
{
//    static uint8_t count = 0;
    uint8_t data = (uint8_t)usart_data_receive(USART0);
//    if(count++ >= 10)
//    {
//        count = 0;
//        DEBUG_RAW("\r\n");
//    }
//    DEBUG_RAW("0x02%x",data);
    lwrb_write (&uart_rb, &data, 1);
}

