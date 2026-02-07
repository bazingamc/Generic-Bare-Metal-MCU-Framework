#pragma once

typedef enum  // PWM channel
{
    PWM_CH1,
    PWM_CH2,
    PWM_CH3,
    PWM_CH4
} PwmChannel;

typedef struct {
    void (*init)(TimerIndex timer, PwmChannel channel, GpioIndex pin, uint8_t gpio_af, uint32_t period, uint32_t prescaler);
    void (*start)(TimerIndex timer, PwmChannel channel);
    void (*stop)(TimerIndex timer, PwmChannel channel);
    void (*set_duty)(TimerIndex timer, PwmChannel channel, uint32_t pulse);
} hal_pwm_ops_t;

extern const hal_pwm_ops_t hal_pwm;
