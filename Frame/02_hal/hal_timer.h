#pragma once


typedef enum  // Timer index
{
    _TIM1,
    _TIM2,
    _TIM3,
    _TIM4,
    _TIM5,
    _TIM6,
    _TIM7,
    _TIM8,
    _TIM9,
    _TIM10,
    _TIM11,
    _TIM12,
    _TIM13,
    _TIM14
} TimerIndex;

typedef struct {
    void (*init)(TimerIndex timer, uint32_t period, uint32_t prescaler);
    void (*start)(TimerIndex timer);
    void (*stop)(TimerIndex timer);
    void (*set_callback)(TimerIndex timer, void (*callback)(void));
} hal_timer_ops_t;

extern const hal_timer_ops_t hal_timer;