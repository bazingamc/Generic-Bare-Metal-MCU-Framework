#include "hal.h"

static void (*systick_callbacks)(void);
void SysTick_Handler(void)
{
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
			                   SysTick_CTRL_TICKINT_Msk   |
			                   SysTick_CTRL_ENABLE_Msk;

    if(systick_callbacks != NULL)     
    {
        systick_callbacks();
    }                      
}

void systick_init(uint32_t timeUs, void (*callback)(void))
{
    SysTick_Config(timeUs*(SYSTEM_CLOCK_MHZ*1000000/1000000));
    systick_callbacks = callback;
}




const hal_systick_ops_t hal_systick = {
    .init = systick_init
};