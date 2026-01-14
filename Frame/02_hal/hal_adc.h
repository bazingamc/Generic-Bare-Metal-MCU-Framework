#pragma once


typedef enum  // ADC编号
{
    _ADC1,
    _ADC2,
    _ADC3
} AdcIndex;

typedef enum  // ADC通道
{
    ADC_CH0 = 0,
    ADC_CH1,
    ADC_CH2,
    ADC_CH3,
    ADC_CH4,
    ADC_CH5,
    ADC_CH6,
    ADC_CH7,
    ADC_CH8,
    ADC_CH9,
    ADC_CH10,
    ADC_CH11,
    ADC_CH12,
    ADC_CH13,
    ADC_CH14,
    ADC_CH15,
    ADC_CH16,
    ADC_CH17,
    ADC_CH18
} AdcChannel;

typedef struct {
    void (*init)(AdcIndex adc);
    uint16_t (*read)(AdcIndex adc, AdcChannel channel);
} hal_adc_ops_t;

extern const hal_adc_ops_t hal_adc;