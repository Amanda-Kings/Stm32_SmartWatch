#ifndef D : _CODE_STM32_SMARTWATCH_BSP_MCU_INC_BSP_SYS_TICK_H
#define D:_CODE_STM32_SMARTWATCH_BSP_MCU_INC_BSP_SYS_TICK_H

#include "stm32f10x_conf.h"

void SysTick_Init(void);
void SysTick_Update(void);
uint32_t SysTick_GetTick(void);

#endif
