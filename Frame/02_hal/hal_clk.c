#include "hal.h"

static int CalcPLL(uint32_t fin, uint32_t sysclk,
                   uint32_t* pllm,
                   uint32_t* plln,
                   uint32_t* pllp)
{
    /* Simplified strategy: prioritize PLLP = 2 */
    *pllp = 2;

    /* Set VCO_IN = 1MHz */
    *pllm = fin / 1000000U;
    if (*pllm < 2 || *pllm > 63) return -1;

    uint32_t vco = sysclk * (*pllp);
    *plln = vco / 1000000U;

    if (*plln < 50 || *plln > 432) return -2;

    return 0;
}

int SystemClock_Init(const SysClockConfig* cfg)
{
    uint32_t fin;
    uint32_t pllm, plln, pllp;

    if (!cfg) return -1;

    RCC_DeInit();

    if (cfg->clk_src == SYSCLK_SRC_HSE)
    {
        RCC_HSEConfig(RCC_HSE_ON);
        if (RCC_WaitForHSEStartUp() != SUCCESS)
            return -2;

        fin = cfg->hse_freq;
        RCC_PLLConfig(RCC_PLLSource_HSE, 0, 0, 0, 0);
    }
    else
    {
        RCC_HSICmd(ENABLE);
        while (RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET);

        fin = 16000000U;
        RCC_PLLConfig(RCC_PLLSource_HSI, 0, 0, 0, 0);
    }

    if (CalcPLL(fin, cfg->sysclk_freq, &pllm, &plln, &pllp) != 0)
        return -3;

    RCC_PLLCmd(DISABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) != RESET);

    RCC_PLLConfig(
        (cfg->clk_src == SYSCLK_SRC_HSE) ? RCC_PLLSource_HSE : RCC_PLLSource_HSI,
        pllm,
        plln,
        pllp,
        7   /* PLLQ, for USB/SDIO, commonly 7 */
    );

    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    FLASH_SetLatency(FLASH_Latency_5);
    FLASH_PrefetchBufferCmd(ENABLE);
    FLASH_InstructionCacheCmd(ENABLE);
    FLASH_DataCacheCmd(ENABLE);

    RCC_HCLKConfig(RCC_SYSCLK_Div1);
    RCC_PCLK1Config(RCC_HCLK_Div4);
    RCC_PCLK2Config(RCC_HCLK_Div2);

    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08);

    return 0;
}