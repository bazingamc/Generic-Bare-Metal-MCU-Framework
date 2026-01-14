#include "hal.h"



// 根据 TimerIndex 枚举获取对应的 TIM 外设
TIM_TypeDef* get_timer_peripheral(TimerIndex timer) {
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

// 获取对应的 RCC 时钟使能寄存器位
uint32_t get_timer_rcc_clock(TimerIndex timer) {
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

// 获取对应的中断通道
IRQn_Type get_timer_irq_channel(TimerIndex timer) {
    switch(timer) {
        case _TIM1: return TIM1_UP_TIM10_IRQn;
        case _TIM2: return TIM2_IRQn;
        case _TIM3: return TIM3_IRQn;
        case _TIM4: return TIM4_IRQn;
        case _TIM5: return TIM5_IRQn;
        case _TIM6: return TIM6_DAC_IRQn;
        case _TIM7: return TIM7_IRQn;
        case _TIM8: return TIM8_UP_TIM13_IRQn;
        case _TIM9: return TIM1_BRK_TIM9_IRQn;
        case _TIM10: return TIM1_UP_TIM10_IRQn;
        case _TIM11: return TIM1_TRG_COM_TIM11_IRQn;
        case _TIM12: return TIM8_BRK_TIM12_IRQn;
        case _TIM13: return TIM8_UP_TIM13_IRQn;
        case _TIM14: return TIM8_TRG_COM_TIM14_IRQn;
        
        default: return 0;
    }
}

// 存储回调函数的数组
static void (*timer_callbacks[14])(void);



// Timer中断处理函数
static void timer_handle_interrupt(TimerIndex timer) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    
    if(tim != NULL && timer_callbacks[timer] != NULL) {
        if(TIM_GetITStatus(tim, TIM_IT_Update) != RESET) {
            timer_callbacks[timer]();
            TIM_ClearITPendingBit(tim, TIM_IT_Update);
        }
    }
}



// Timer1-7中断处理函数
void TIM1_UP_TIM10_IRQHandler(void) {
    timer_handle_interrupt(_TIM1);
}

void TIM2_IRQHandler(void) {
    timer_handle_interrupt(_TIM2);
}

void TIM3_IRQHandler(void) {
    timer_handle_interrupt(_TIM3);
}

void TIM4_IRQHandler(void) {
    timer_handle_interrupt(_TIM4);
}

void TIM5_IRQHandler(void) {
    timer_handle_interrupt(_TIM5);
}

void TIM6_DAC_IRQHandler(void) {
    timer_handle_interrupt(_TIM6);
}

void TIM7_IRQHandler(void) {
    timer_handle_interrupt(_TIM7);
}

// Timer8中断处理函数
void TIM8_UP_TIM13_IRQHandler(void) {
    timer_handle_interrupt(_TIM8);
}

void TIM8_BRK_TIM12_IRQHandler(void) {
    timer_handle_interrupt(_TIM12);
}

void TIM8_TRG_COM_TIM14_IRQHandler(void) {
    timer_handle_interrupt(_TIM14);
}

void TIM1_BRK_TIM9_IRQHandler(void) {
    timer_handle_interrupt(_TIM9);
}

void TIM1_TRG_COM_TIM11_IRQHandler(void) {
    timer_handle_interrupt(_TIM11);
}


static void timer_init(TimerIndex timer, uint32_t period, uint32_t prescaler) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    uint32_t tim_clock = get_timer_rcc_clock(timer);
    IRQn_Type irq_channel = get_timer_irq_channel(timer);
    
    if (tim == NULL) {
        return;  // 无效的 Timer
    }
    
    // 使能 Timer 时钟
    if (timer <= _TIM7 || timer >= _TIM12) {
        RCC_APB1PeriphClockCmd(tim_clock, ENABLE);
    } else {
        RCC_APB2PeriphClockCmd(tim_clock, ENABLE);
    }
    
    // 初始化 Timer
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
    TIM_TimeBaseStructInit(&TIM_TimeBaseInitStruct);
    
    TIM_TimeBaseInitStruct.TIM_Period = period;
    TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
    TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
    
    TIM_TimeBaseInit(tim, &TIM_TimeBaseInitStruct);
    
    // 清除更新中断标志
    TIM_ClearITPendingBit(tim, TIM_IT_Update);
    
    // 使能更新中断
    TIM_ITConfig(tim, TIM_IT_Update, ENABLE);
    
    // 设置中断优先级
    NVIC_InitTypeDef NVIC_InitStructure;
    NVIC_InitStructure.NVIC_IRQChannel = irq_channel;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 存储回调函数为空
    timer_callbacks[timer] = NULL;
}

static void timer_start(TimerIndex timer) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    
    if (tim != NULL) {
        TIM_Cmd(tim, ENABLE);
    }
}

static void timer_stop(TimerIndex timer) {
    TIM_TypeDef* tim = get_timer_peripheral(timer);
    
    if (tim != NULL) {
        TIM_Cmd(tim, DISABLE);
    }
}

static void timer_set_callback(TimerIndex timer, void (*callback)(void)) {
    if (timer < 14) {
        timer_callbacks[timer] = callback;
    }
}

const hal_timer_ops_t hal_timer = {
    .init = timer_init,
    .start = timer_start,
    .stop = timer_stop,
    .set_callback = timer_set_callback
};