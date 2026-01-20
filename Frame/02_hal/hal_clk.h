#pragma once

typedef enum
{
    SYSCLK_SRC_HSI = 0,   // Internal high-speed clock 16MHz
    SYSCLK_SRC_HSE        // External high-speed crystal
} SysClkSource;

typedef struct
{
    SysClkSource clk_src;   // Clock source selection
    uint32_t hse_freq;      // External crystal frequency (Hz), valid when using HSE
    uint32_t sysclk_freq;   // Target system clock frequency (Hz)
} SysClockConfig;

int SystemClock_Init(const SysClockConfig* cfg);