#include "hal.h"



// Get the corresponding ADC peripheral according to AdcIndex enumeration
ADC_TypeDef* get_adc_peripheral(AdcIndex adc) {
    switch(adc) {
        case _ADC1: return ADC1;
        case _ADC2: return ADC2;
        case _ADC3: return ADC3;
        default: return NULL;
    }
}

// Get the corresponding RCC clock enable register bit
uint32_t get_adc_rcc_clock(AdcIndex adc) {
    switch(adc) {
        case _ADC1: return RCC_APB2Periph_ADC1;
        case _ADC2: return RCC_APB2Periph_ADC2;
        case _ADC3: return RCC_APB2Periph_ADC3;
        default: return 0;
    }
}

// Convert GPIO pin to ADC channel
AdcChannel get_adc_channel_from_gpio(GpioIndex pin) {
    // STM32F4 ADC channel mapping
    // Here we assume only common pin mappings are handled
    switch(pin) {
        case PA0: return ADC_CH0;
        case PA1: return ADC_CH1;
        case PA2: return ADC_CH2;
        case PA3: return ADC_CH3;
        case PA4: return ADC_CH4;
        case PA5: return ADC_CH5;
        case PA6: return ADC_CH6;
        case PA7: return ADC_CH7;
        case PB0: return ADC_CH8;
        case PB1: return ADC_CH9;
        case PC0: return ADC_CH10;
        case PC1: return ADC_CH11;
        case PC2: return ADC_CH12;
        case PC3: return ADC_CH13;
        case PC4: return ADC_CH14;
        case PC5: return ADC_CH15;
        default: return ADC_CH0;  // Default return ADC_CH0
    }
}

// Get GPIO pin corresponding to ADC channel
GpioIndex get_gpio_from_adc_channel(AdcChannel channel) {
    // Simple mapping relationship, may be more complex in actual applications
    switch(channel) {
        case ADC_CH0: return PA0;
        case ADC_CH1: return PA1;
        case ADC_CH2: return PA2;
        case ADC_CH3: return PA3;
        case ADC_CH4: return PA4;
        case ADC_CH5: return PA5;
        case ADC_CH6: return PA6;
        case ADC_CH7: return PA7;
        case ADC_CH8: return PB0;
        case ADC_CH9: return PB1;
        case ADC_CH10: return PC0;
        case ADC_CH11: return PC1;
        case ADC_CH12: return PC2;
        case ADC_CH13: return PC3;
        case ADC_CH14: return PC4;
        case ADC_CH15: return PC5;
        default: return PA0;  // Default return PA0
    }
}

// Configure GPIO pins for ADC
void configure_adc_gpio(AdcChannel channel) {
    GpioIndex pin = get_gpio_from_adc_channel(channel);
    GPIO_TypeDef* port = get_gpio_port(pin);
    uint16_t pin_num = get_gpio_pin(pin);
    
    // Enable GPIO port clock
    uint32_t gpio_clock = get_gpio_rcc_clock(pin);
    RCC_AHB1PeriphClockCmd(gpio_clock, ENABLE);
    
    // Configure pin as analog input
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;      // Analog input mode
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // No pull-up/pull-down
    GPIO_Init(port, &GPIO_InitStruct);
}

static void adc_init(AdcIndex adc) {
    ADC_TypeDef* adc_dev = get_adc_peripheral(adc);
    uint32_t adc_clock = get_adc_rcc_clock(adc);
    
    if (adc_dev == NULL) {
        return;  // Invalid ADC
    }
    
    // Enable ADC clock
    RCC_APB2PeriphClockCmd(adc_clock, ENABLE);
    
    // Initialize ADC
    ADC_InitTypeDef ADC_InitStruct;
    ADC_StructInit(&ADC_InitStruct);
    
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;         // 12-bit resolution
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;                  // Disable scan mode
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;            // Disable continuous conversion
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // No external trigger
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;         // Right alignment
    ADC_InitStruct.ADC_NbrOfConversion = 1;                     // 1 conversion
    
    ADC_Init(adc_dev, &ADC_InitStruct);
    
    // Configure ADC clock
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    ADC_CommonStructInit(&ADC_CommonInitStruct);
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;     // 4-divider
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // Sampling delay
    ADC_CommonInit(&ADC_CommonInitStruct);
    
    // Enable ADC
    ADC_Cmd(adc_dev, ENABLE);
}

static uint16_t adc_read(AdcIndex adc, AdcChannel channel) {
    ADC_TypeDef* adc_dev = get_adc_peripheral(adc);
    
    if (adc_dev == NULL) {
        return 0;  // Invalid ADC
    }
    
    // Configure GPIO as analog input
    configure_adc_gpio(channel);
    
    // Configure channel
    ADC_RegularChannelConfig(adc_dev, channel, 1, ADC_SampleTime_480Cycles);  // Longest sampling time for best accuracy
    
    // Start conversion
    ADC_SoftwareStartConv(adc_dev);
    
    // Wait for conversion completion
    while(ADC_GetFlagStatus(adc_dev, ADC_FLAG_EOC) == RESET);
    
    // Read result
    return ADC_GetConversionValue(adc_dev);
}

const hal_adc_ops_t hal_adc = {
    .init = adc_init,
    .read = adc_read
};