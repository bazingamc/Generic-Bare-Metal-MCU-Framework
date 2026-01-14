#include "hal.h"

void dwt_init()
{
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    DWT->CYCCNT = 0;
}
void dwt_delay(uint32_t timeUs)
{
    uint32_t systick_stmp;
    systick_stmp = DWT->CYCCNT;
    timeUs = timeUs * SYSTEM_CLOCK_MHZ;
    while (DWT->CYCCNT - systick_stmp < timeUs)
    {
    };
}

uint32_t dwt_getCYCCNT()
{
    return DWT->CYCCNT;
}

const hal_dwt_ops_t hal_dwt = {
    .init = dwt_init,
    .delay = dwt_delay,
    .getCYCCNT = dwt_getCYCCNT,
};