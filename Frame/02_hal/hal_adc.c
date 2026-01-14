#include "hal.h"



// 根据 AdcIndex 枚举获取对应的 ADC 外设
ADC_TypeDef* get_adc_peripheral(AdcIndex adc) {
    switch(adc) {
        case _ADC1: return ADC1;
        case _ADC2: return ADC2;
        case _ADC3: return ADC3;
        default: return NULL;
    }
}

// 获取对应的 RCC 时钟使能寄存器位
uint32_t get_adc_rcc_clock(AdcIndex adc) {
    switch(adc) {
        case _ADC1: return RCC_APB2Periph_ADC1;
        case _ADC2: return RCC_APB2Periph_ADC2;
        case _ADC3: return RCC_APB2Periph_ADC3;
        default: return 0;
    }
}

// 将GPIO引脚转换为ADC通道
AdcChannel get_adc_channel_from_gpio(GpioIndex pin) {
    // STM32F4的ADC通道映射
    // 这里假设我们只处理常见的引脚映射
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
        default: return ADC_CH0;  // 默认返回ADC_CH0
    }
}

// 获取ADC通道对应的GPIO引脚
GpioIndex get_gpio_from_adc_channel(AdcChannel channel) {
    // 简单的映射关系，实际应用中可能更复杂
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
        default: return PA0;  // 默认返回PA0
    }
}

// 配置 ADC 的 GPIO 引脚
void configure_adc_gpio(AdcChannel channel) {
    GpioIndex pin = get_gpio_from_adc_channel(channel);
    GPIO_TypeDef* port = get_gpio_port(pin);
    uint16_t pin_num = get_gpio_pin(pin);
    
    // 使能 GPIO 端口时钟
    uint32_t gpio_clock = get_gpio_rcc_clock(pin);
    RCC_AHB1PeriphClockCmd(gpio_clock, ENABLE);
    
    // 配置引脚为模拟输入
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = pin_num;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;      // 模拟输入模式
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;  // 无上下拉
    GPIO_Init(port, &GPIO_InitStruct);
}

static void adc_init(AdcIndex adc) {
    ADC_TypeDef* adc_dev = get_adc_peripheral(adc);
    uint32_t adc_clock = get_adc_rcc_clock(adc);
    
    if (adc_dev == NULL) {
        return;  // 无效的 ADC
    }
    
    // 使能 ADC 时钟
    RCC_APB2PeriphClockCmd(adc_clock, ENABLE);
    
    // 初始化 ADC
    ADC_InitTypeDef ADC_InitStruct;
    ADC_StructInit(&ADC_InitStruct);
    
    ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;         // 12位分辨率
    ADC_InitStruct.ADC_ScanConvMode = DISABLE;                  // 禁用扫描模式
    ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;            // 禁用连续转换
    ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 不使用外部触发
    ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;         // 右对齐
    ADC_InitStruct.ADC_NbrOfConversion = 1;                     // 1个转换
    
    ADC_Init(adc_dev, &ADC_InitStruct);
    
    // 配置ADC时钟
    ADC_CommonInitTypeDef ADC_CommonInitStruct;
    ADC_CommonStructInit(&ADC_CommonInitStruct);
    ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div4;     // 4分频
    ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // 采样延时
    ADC_CommonInit(&ADC_CommonInitStruct);
    
    // 使能 ADC
    ADC_Cmd(adc_dev, ENABLE);
}

static uint16_t adc_read(AdcIndex adc, AdcChannel channel) {
    ADC_TypeDef* adc_dev = get_adc_peripheral(adc);
    
    if (adc_dev == NULL) {
        return 0;  // 无效的 ADC
    }
    
    // 配置GPIO为模拟输入
    configure_adc_gpio(channel);
    
    // 配置通道
    ADC_RegularChannelConfig(adc_dev, channel, 1, ADC_SampleTime_480Cycles);  // 最长采样时间以获得最佳精度
    
    // 开始转换
    ADC_SoftwareStartConv(adc_dev);
    
    // 等待转换完成
    while(ADC_GetFlagStatus(adc_dev, ADC_FLAG_EOC) == RESET);
    
    // 读取结果
    return ADC_GetConversionValue(adc_dev);
}

const hal_adc_ops_t hal_adc = {
    .init = adc_init,
    .read = adc_read
};