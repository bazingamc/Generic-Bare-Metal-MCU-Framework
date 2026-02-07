#include "hal.h"

static TIM_TypeDef* get_timer_peripheral(TimerIndex timer) {
    switch(timer) {
        case _TIM1: return TIM1;
        case _TIM2: return TIM2;
        case _TIM3: return TIM3;
        case _TIM4: return TIM4;
        case _TIM5: return TIM5;
        case _TIM6: return TIM6;
        case _TIM7: return TIM7;
        case _TIM8: return TIM8;
        case _TIM9: return TIM9;
        case _TIM10: return TIM10;
        case _TIM11: return TIM11;
        case _TIM12: return TIM12;
        case _TIM13: return TIM13;
        case _TIM14: return TIM14;
        default: return NULL;
    }
}

static uint32_t get_timer_rcc_clock(TimerIndex timer) {
    switch(timer) {
        case _TIM1: return RCC_APB2Periph_TIM1;
        case _TIM2: return RCC_APB1Periph_TIM2;
        case _TIM3: return RCC_APB1Periph_TIM3;
        case _TIM4: return RCC_APB1Periph_TIM4;
        case _TIM5: return RCC_APB1Periph_TIM5;
        case _TIM6: return RCC_APB1Periph_TIM6;
        case _TIM7: return RCC_APB1Periph_TIM7;
        case _TIM8: return RCC_APB2Periph_TIM8;
        case _TIM9: return RCC_APB2Periph_TIM9;
        case _TIM10: return RCC_APB2Periph_TIM10;
        case _TIM11: return RCC_APB2Periph_TIM11;
        case _TIM12: return RCC_APB1Periph_TIM12;
        case _TIM13: return RCC_APB1Periph_TIM13;
        case _TIM14: return RCC_APB1Periph_TIM14;
        default: return 0;
    }
}

static uint16_t get_timer_channel(PwmChannel channel) {
    switch(channel) {
        case PWM_CH1: return TIM_Channel_1;
        case PWM_CH2: return TIM_Channel_2;
        case PWM_CH3: return TIM_Channel_3;
        case PWM_CH4: return TIM_Channel_4;
        default: return 0;
    }
}

static GPIO_TypeDef* get_pwm_gpio_port(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return GPIOA;
    } else if (pin >= PB0 && pin <= PB15) {
        return GPIOB;
    } else if (pin >= PC0 && pin <= PC15) {
        return GPIOC;
    } else if (pin >= PD0 && pin <= PD15) {
        return GPIOD;
    } else if (pin >= PE0 && pin <= PE15) {
        return GPIOE;
    } else if (pin >= PF0 && pin <= PF15) {
        return GPIOF;
    } else if (pin >= PG0 && pin <= PG15) {
        return GPIOG;
    } else if (pin >= PH0 && pin <= PH15) {
        return GPIOH;
    } else if (pin >= PI0 && pin <= PI15) {
        return GPIOI;
    }
    return NULL;
}

static uint16_t get_pwm_gpio_pin(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return 1 << (pin - PA0);
    } else if (pin >= PB0 && pin <= PB15) {
        return 1 << (pin - PB0);
    } else if (pin >= PC0 && pin <= PC15) {
        return 1 << (pin - PC0);
    } else if (pin >= PD0 && pin <= PD15) {
        return 1 << (pin - PD0);
    } else if (pin >= PE0 && pin <= PE15) {
        return 1 << (pin - PE0);
    } else if (pin >= PF0 && pin <= PF15) {
        return 1 << (pin - PF0);
    } else if (pin >= PG0 && pin <= PG15) {
        return 1 << (pin - PG0);
    } else if (pin >= PH0 && pin <= PH15) {
        return 1 << (pin - PH0);
    } else if (pin >= PI0 && pin <= PI15) {
        return 1 << (pin - PI0);
    }
    return 0;
}

static uint32_t get_pwm_gpio_rcc_clock(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return RCC_AHB1Periph_GPIOA;
    } else if (pin >= PB0 && pin <= PB15) {
        return RCC_AHB1Periph_GPIOB;
    } else if (pin >= PC0 && pin <= PC15) {
        return RCC_AHB1Periph_GPIOC;
    } else if (pin >= PD0 && pin <= PD15) {
        return RCC_AHB1Periph_GPIOD;
    } else if (pin >= PE0 && pin <= PE15) {
        return RCC_AHB1Periph_GPIOE;
    } else if (pin >= PF0 && pin <= PF15) {
        return RCC_AHB1Periph_GPIOF;
    } else if (pin >= PG0 && pin <= PG15) {
        return RCC_AHB1Periph_GPIOG;
    } else if (pin >= PH0 && pin <= PH15) {
        return RCC_AHB1Periph_GPIOH;
    } else if (pin >= PI0 && pin <= PI15) {
        return RCC_AHB1Periph_GPIOI;
    }
    return 0;
}

static uint8_t get_gpio_pin_source(GpioIndex pin) {
    if (pin >= PA0 && pin <= PA15) {
        return (uint8_t)(pin - PA0);
    } else if (pin >= PB0 && pin <= PB15) {
        return (uint8_t)(pin - PB0);
    } else if (pin >= PC0 && pin <= PC15) {
        return (uint8_t)(pin - PC0);
    } else if (pin >= PD0 && pin <= PD15) {
        return (uint8_t)(pin - PD0);
    } else if (pin >= PE0 && pin <= PE15) {
        return (uint8_t)(pin - PE0);
    } else if (pin >= PF0 && pin <= PF15) {
        return (uint8_t)(pin - PF0);
    } else if (pin >= PG0 && pin <= PG15) {
        return (uint8_t)(pin - PG0);
    } else if (pin >= PH0 && pin <= PH15) {
        return (uint8_t)(pin - PH0);
    } else if (pin >= PI0 && pin <= PI15) {
        return (uint8_t)(pin - PI0);
    }
    return 0;
}

static int pwm_channel_supported(TimerIndex timer, PwmChannel channel) {
    switch (timer) {
        case _TIM1:
        case _TIM2:
        case _TIM3:
        case _TIM4:
        case _TIM5:
        case _TIM8:
            return (channel == PWM_CH1 || channel == PWM_CH2 || channel == PWM_CH3 || channel == PWM_CH4);
        case _TIM9:
            return (channel == PWM_CH1 || channel == PWM_CH2);
        case _TIM10:
        case _TIM11:
        case _TIM13:
        case _TIM14:
            return (channel == PWM_CH1);
        case _TIM12:
            return (channel == PWM_CH1 || channel == PWM_CH2);
        default:
            return 0;
    }
}

static void pwm_init(TimerIndex timer, PwmChannel channel, GpioIndex pin, uint8_t gpio_af, uint32_t period, uint32_t prescaler) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    uint32_t tim_clock = get_timer_rcc_clock(timer);

    if (tim == NULL || tim_clock == 0 || !pwm_channel_supported(timer, channel)) {
        return;
    }

    // Enable timer clock
    if (timer <= _TIM7 || timer >= _TIM12) {
        RCC_APB1PeriphClockCmd(tim_clock, ENABLE);
    } else {
        RCC_APB2PeriphClockCmd(tim_clock, ENABLE);
    }

    // GPIO AF configuration
    GPIO_TypeDef* port = get_pwm_gpio_port(pin);
    uint16_t pin_number = get_pwm_gpio_pin(pin);
    uint32_t gpio_clock = get_pwm_gpio_rcc_clock(pin);
    uint8_t pin_source = get_gpio_pin_source(pin);

    if (port == NULL || pin_number == 0) {
        return;
    }

    RCC_AHB1PeriphClockCmd(gpio_clock, ENABLE);

    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_StructInit(&GPIO_InitStruct);
    GPIO_InitStruct.GPIO_Pin = pin_number;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(port, &GPIO_InitStruct);
    GPIO_PinAFConfig(port, pin_source, gpio_af);

    // Timer base configuration
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    TIM_TimeBaseInitStruct.TIM_Period = period;
    TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(tim, &TIM_TimeBaseInitStruct);
    TIM_ARRPreloadConfig(tim, ENABLE);

    // PWM channel configuration
    TIM_OCInitTypeDef TIM_OCInitStruct;
    TIM_OCStructInit(&TIM_OCInitStruct);
    TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStruct.TIM_Pulse = 0;
    TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

    switch (channel) {
        case PWM_CH1:
            TIM_OC1Init(tim, &TIM_OCInitStruct);
            TIM_OC1PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        case PWM_CH2:
            TIM_OC2Init(tim, &TIM_OCInitStruct);
            TIM_OC2PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        case PWM_CH3:
            TIM_OC3Init(tim, &TIM_OCInitStruct);
            TIM_OC3PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        case PWM_CH4:
            TIM_OC4Init(tim, &TIM_OCInitStruct);
            TIM_OC4PreloadConfig(tim, TIM_OCPreload_Enable);
            break;
        default:
            return;
    }

    if (timer == _TIM1 || timer == _TIM8) {
        TIM_CtrlPWMOutputs(tim, ENABLE);
    }
}

static void pwm_start(TimerIndex timer, PwmChannel channel) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    uint16_t tim_channel = get_timer_channel(channel);

    if (tim != NULL && tim_channel != 0 && pwm_channel_supported(timer, channel)) {
        TIM_CCxCmd(tim, tim_channel, ENABLE);
        TIM_Cmd(tim, ENABLE);
    }
}

static void pwm_stop(TimerIndex timer, PwmChannel channel) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    uint16_t tim_channel = get_timer_channel(channel);

    if (tim != NULL && tim_channel != 0 && pwm_channel_supported(timer, channel)) {
        TIM_CCxCmd(tim, tim_channel, DISABLE);
    }
}

static void pwm_set_duty(TimerIndex timer, PwmChannel channel, uint32_t pulse) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);

    if (tim == NULL || !pwm_channel_supported(timer, channel)) {
        return;
    }

    switch (channel) {
        case PWM_CH1: TIM_SetCompare1(tim, pulse); break;
        case PWM_CH2: TIM_SetCompare2(tim, pulse); break;
        case PWM_CH3: TIM_SetCompare3(tim, pulse); break;
        case PWM_CH4: TIM_SetCompare4(tim, pulse); break;
        default: break;
    }
}

const hal_pwm_ops_t hal_pwm = {
    .init = pwm_init,
    .start = pwm_start,
    .stop = pwm_stop,
    .set_duty = pwm_set_duty
};
