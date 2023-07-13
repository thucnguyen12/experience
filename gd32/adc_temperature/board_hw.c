#include "board_hw.h"
#include "gd32f3x0.h"
#include "gd32f3x0_usart.h"

//#include "app_debug.h"
#include "main.h"
#include "math.h"
#include <math.h>
#include <stdbool.h>

#define RTC_BKP_VALUE    0x32F0

#ifndef __ALIGNED
#define _align
#endif


static void config_led_pwm(void);
void board_hw_initialize(void)
{
    //nvic_irq_enable(LVD_IRQn, 1);
    /* RCC configurations */    
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOF);
    //rcu_periph_clock_enable(RCU_PMU);
    
    	/* Init io */
	// Step 1 All LCD Output IO
    gpio_mode_set(GPIO_EN_PWR_LCD_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_EN_PWR_LCD_PIN);
    gpio_output_options_set(GPIO_EN_PWR_LCD_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_EN_PWR_LCD_PIN);
    gpio_bit_reset(GPIO_EN_PWR_LCD_PORT, GPIO_EN_PWR_LCD_PIN);
    
	gpio_mode_set(GPIO_LCD_DI_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_DI_PIN);
    gpio_output_options_set(GPIO_LCD_DI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_DI_PIN);
    gpio_bit_reset(GPIO_LCD_DI_PORT, GPIO_LCD_DI_PIN);
    
	gpio_mode_set(GPIO_LCD_RW_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_RW_PIN);
    gpio_output_options_set(GPIO_LCD_RW_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_RW_PIN);
    gpio_bit_reset(GPIO_LCD_RW_PORT, GPIO_LCD_RW_PIN);
    
    gpio_mode_set(GPIO_LCD_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_EN_PIN);
    gpio_output_options_set(GPIO_LCD_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_EN_PIN);
    gpio_bit_reset(GPIO_LCD_EN_PORT, GPIO_LCD_EN_PIN);
    
    gpio_mode_set(GPIO_LCD_D0_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D0_PIN);
    gpio_output_options_set(GPIO_LCD_D0_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D0_PIN);
    gpio_bit_reset(GPIO_LCD_D0_PORT, GPIO_LCD_D0_PIN);
    
    gpio_mode_set(GPIO_LCD_D1_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D1_PIN);
    gpio_output_options_set(GPIO_LCD_D1_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D1_PIN);
    gpio_bit_reset(GPIO_LCD_D1_PORT, GPIO_LCD_D1_PIN);
    
    gpio_mode_set(GPIO_LCD_D2_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D2_PIN);
    gpio_output_options_set(GPIO_LCD_D2_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D2_PIN);
    gpio_bit_reset(GPIO_LCD_D2_PORT, GPIO_LCD_D2_PIN);
    
    gpio_mode_set(GPIO_LCD_D3_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D3_PIN);
    gpio_output_options_set(GPIO_LCD_D3_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D3_PIN);
    gpio_bit_reset(GPIO_LCD_D3_PORT, GPIO_LCD_D3_PIN);
    
    gpio_mode_set(GPIO_LCD_D4_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D4_PIN);
    gpio_output_options_set(GPIO_LCD_D4_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D4_PIN);
    gpio_bit_set(GPIO_LCD_D4_PORT, GPIO_LCD_D4_PIN);
    
    gpio_mode_set(GPIO_LCD_D5_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D5_PIN);
    gpio_output_options_set(GPIO_LCD_D5_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D5_PIN);
    gpio_bit_reset(GPIO_LCD_D5_PORT, GPIO_LCD_D5_PIN);
    
    gpio_mode_set(GPIO_LCD_D6_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D6_PIN);
    gpio_output_options_set(GPIO_LCD_D6_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D6_PIN);
    gpio_bit_reset(GPIO_LCD_D6_PORT, GPIO_LCD_D6_PIN);
    
    gpio_mode_set(GPIO_LCD_D7_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_D7_PIN);
    gpio_output_options_set(GPIO_LCD_D7_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_D7_PIN);
    gpio_bit_reset(GPIO_LCD_D7_PORT, GPIO_LCD_D7_PIN);
    
    gpio_mode_set(GPIO_LCD_RST_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_RST_PIN);
    gpio_output_options_set(GPIO_LCD_RST_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_RST_PIN);
    gpio_bit_reset(GPIO_LCD_RST_PORT, GPIO_LCD_RST_PIN);
    
    gpio_mode_set(GPIO_LCD_PWM_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LCD_PWM_PIN);
    gpio_output_options_set(GPIO_LCD_PWM_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LCD_PWM_PIN);
    gpio_bit_reset(GPIO_LCD_PWM_PORT, GPIO_LCD_PWM_PIN);
    
    //GPIO 
    gpio_mode_set (GPIO_ESP_EN_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE ,GPIO_ESP_EN_PIN);
    //gpio_mode_set (GPIO_ESP_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP ,GPIO_ESP_EN_PIN);
   // gpio_output_options_set(GPIO_ESP_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_ESP_EN_PIN);
    
    //spi gpio
    gpio_mode_set(GPIO_SPI0_SCK_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SPI0_SCK_PIN);
    gpio_output_options_set(GPIO_SPI0_SCK_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_SPI0_SCK_PIN);
    
    gpio_mode_set(GPIO_SPI0_MISO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SPI0_MISO_PIN);
    gpio_output_options_set(GPIO_SPI0_MISO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_SPI0_MISO_PIN);
    
    gpio_mode_set(GPIO_SPI0_MOSI_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_SPI0_MOSI_PIN);
    gpio_output_options_set(GPIO_SPI0_MOSI_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_SPI0_MOSI_PIN);
    
    gpio_af_set(GPIO_SPI0_SCK_PORT, GPIO_AF_0, GPIO_SPI0_SCK_PIN);
    gpio_af_set(GPIO_SPI0_MISO_PORT, GPIO_AF_0, GPIO_SPI0_MISO_PIN);
    gpio_af_set(GPIO_SPI0_MOSI_PORT, GPIO_AF_0, GPIO_SPI0_MOSI_PIN);
    
    //spi cs
    gpio_mode_set(GPIO_NRF_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_NRF_CS_PIN);
    gpio_output_options_set(GPIO_NRF_CS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_NRF_CS_PIN);
    

    //lora
//    gpio_mode_set(GPIO_LORA_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_LORA_CS_PIN);
//    gpio_output_options_set(GPIO_LORA_CS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LORA_CS_PIN);
//    
//    gpio_mode_set(GPIO_LORA_IO4_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_LORA_IO4_PIN);
//    gpio_output_options_set(GPIO_LORA_IO4_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LORA_IO4_PIN);
//    gpio_mode_set(GPIO_LORA_EN_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, GPIO_LORA_EN_PIN);
//    gpio_output_options_set(GPIO_LORA_EN_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_LORA_EN_PIN);
//    
//    //zigbee
//    gpio_mode_set(GPIO_ZIGBEE_CS_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO_ZIGBEE_CS_PIN);
//    gpio_output_options_set(GPIO_ZIGBEE_CS_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_ZIGBEE_CS_PIN);
    
    //NRF ping
    gpio_mode_set (GPIO_NRF_PING_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE ,GPIO_NRF_PING_PIN);
    //gpio_output_options_set(GPIO_NRF_PING_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_NRF_PING_PIN);
    
    //uart config
    usart_deinit(USART0);
    rcu_periph_clock_enable(RCU_USART0);
    
    gpio_af_set(GPIO_UART_PORT, GPIO_AF_1, GPIO_ESP_RX_PIN);
    gpio_af_set(GPIO_UART_PORT, GPIO_AF_1, GPIO_ESP_TX_PIN);
    
    gpio_mode_set(GPIO_UART_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_ESP_RX_PIN);
    gpio_output_options_set(GPIO_UART_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_ESP_RX_PIN);
    
    gpio_mode_set(GPIO_UART_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_ESP_TX_PIN);
    gpio_output_options_set(GPIO_UART_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_ESP_TX_PIN);
    
    
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);
    usart_interrupt_enable(USART0, USART_INT_RBNE);
	nvic_irq_enable(USART0_IRQn, 2, 0);
    
    //rs232 config
    rcu_periph_clock_enable(RCU_USART1);
    gpio_af_set(GPIO_RS232_PORT, GPIO_AF_1, GPIO_RS232_RX_PIN);
    gpio_af_set(GPIO_RS232_PORT, GPIO_AF_1, GPIO_RS232_TX_PIN);
    gpio_mode_set(GPIO_RS232_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_RS232_RX_PIN);
    gpio_output_options_set(GPIO_RS232_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_RS232_RX_PIN);
    gpio_mode_set(GPIO_RS232_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_RS232_TX_PIN);
    gpio_output_options_set(GPIO_RS232_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO_RS232_TX_PIN);
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

    usart_enable(USART1);
    usart_interrupt_enable(USART1, USART_INT_RBNE);
	// usart_interrupt_enable(USART1, USART_INT_TBE);
	nvic_irq_enable(USART1_IRQn, 1, 0);

}

volatile uint32_t m_delay_remain;
void sys_delay_ms(uint32_t ms)
{
    m_delay_remain = ms;
    while (m_delay_remain)
    {
        
    }
}
#define MAX_ADC_CHANNEL 1

uint16_t m_adc_value[MAX_ADC_CHANNEL];
uint32_t m_adc_index = 0;

void board_adc_init(void)
{
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);
    
    gpio_mode_set(GPIO_ZONE_ADC_PORT, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_ZONE_ADC_PIN);
//    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_1);
//    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, GPIO_PIN_2);
    
    dma_parameter_struct dma_data_parameter;
    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(m_adc_value);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = 1U;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA_CH0);
    
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    
    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE); 
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 1U);//3U);

    /* ADC regular channel config */
    adc_regular_channel_config(0U, ADC_CHANNEL_6, ADC_SAMPLETIME_239POINT5);
//    adc_regular_channel_config(1U, ADC_CHANNEL_1, ADC_SAMPLETIME_239POINT5);
//    adc_regular_channel_config(2U, ADC_CHANNEL_2, ADC_SAMPLETIME_239POINT5);

    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

//    /* ADC discontinuous mode */
//    adc_discontinuous_mode_config(ADC_REGULAR_CHANNEL, 3U);

    /* enable ADC interface */
    adc_enable();
    //sys_delay_ms(2U);

    /* ADC calibration and reset calibration */
    adc_calibration_enable();

    /* ADC DMA function enable */
    adc_dma_mode_enable();
    
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}

uint32_t  board_hw_read_adc_ntc (void)
{
    uint32_t tmp = m_adc_value[0];
    tmp = (tmp*3300);
    tmp = tmp / 4095;
    
    return tmp;
}

static bool convert_temperature(uint32_t vtemp_mv, uint32_t vbat_mv, int32_t *result)
{
    #define HW_RESISTOR_SERIES_NTC 10000 //10K Ohm
    /* This is thermistor dependent ` and it should be in the datasheet, or refer to the
    article for how to calculate it using the Beta equation.
    I had to do this, but I would try to get a thermistor with a known
    beta if you want to avoid empirical calculations. */
    #define BETA 3470.0f //B25/B75

    /* This is also needed for the conversion equation as "typical" room temperature
    is needed as an input. */
    #define ROOM_TEMP 298.15f // room temperature in Kelvin

    /* Thermistors will have a typical resistance at room temperature so write this 
    down here. Again, needed for conversion equations. */
    #define RESISTOR_ROOM_TEMP 10000.0f //Resistance in Ohms @ 25oC	330k/470k

    bool retval = true;
    float vtemp_float;
    float vbat_float;
    float r_ntc;
    float temp ;
    
    if(vtemp_mv == vbat_mv
        || vtemp_mv == 0)
    {
        retval = false;
        goto end;
    }

    //NTC ADC: Vntc1 = Vntc2 = Vin
    //Tinh dien ap NTC
    vtemp_float = vtemp_mv / 1000.0f;
    vbat_float = vbat_mv / 1000.0f;

    //Caculate NTC resistor : Vntc = Vin*Rs/(Rs + Rntc) -> Rntc = Rs(Vin/Vntc - 1)
    r_ntc = HW_RESISTOR_SERIES_NTC*(vbat_float/vtemp_float - 1);

    if (r_ntc <= 0)
    {
        retval = false;
        goto end;
    }

    temp = (BETA * ROOM_TEMP) / (BETA + (ROOM_TEMP * log(r_ntc / RESISTOR_ROOM_TEMP))) - 273.15f;

    if (temp < 0)
    {
        retval = false;
        goto end;
    }

    *result = (int32_t)temp;
end:
    return retval;
}

int32_t temp = 0;

uint32_t get_temperature (void)
{
    uint32_t Vntc = board_hw_read_adc_ntc();
    convert_temperature (Vntc, 3300, &temp);
    return (uint32_t)temp;
}

void board_hw_reset(void)
{
    NVIC_SystemReset();
}

//void systime_update()
//{
//    systime_set_value(tt);
//}


//__align(4) uint16_t m_adc_value[1];

