#include "bsp_sys_tick.h"

static volatile uint32_t tick_count = 0;

void SysTick_Init(void){
    // 配置SysTick定时器，假设系统时钟为72MHz，每1ms产生一次中断
    SysTick->VAL = 0;
    SysTick->LOAD = (SystemCoreClock / 1000) - 1;
    NVIC_SetPriority(SysTick_IRQn, (1 << __NVIC_PRIO_BITS) - 1); // 设置最低优先级
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}
uint32_t SysTick_GetTick(void){
    return tick_count;
}
void SysTick_Update(void)
{
    tick_count++;
}