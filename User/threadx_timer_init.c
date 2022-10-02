#include "ch32v30x.h"

#define TICK_PER_SECOND 1000
void init_timer(void)
{
	NVIC_SetPriority(SysTicK_IRQn, 0xf0);
	NVIC_EnableIRQ(SysTicK_IRQn);

	SysTick->CTLR = 0;
	SysTick->SR = 0;
	SysTick->CNT = 0;
	SysTick->CMP = SystemCoreClock / TICK_PER_SECOND;
	SysTick->CTLR = 0xf;
}

void clr_systick_sr(void)
{
	SysTick->SR = 0;
}
