#ifndef D : _CODE_STM32_SMARTWATCH_BSP_MCU_INC_BSP_UART_H
#define D:_CODE_STM32_SMARTWATCH_BSP_MCU_INC_BSP_UART_H

#include "stm32f10x_conf.h"

void UART1_Init(void);
void UART1_SendByte(uint8_t byte);
// void UART1_SendString(const char* str);
// uint8_t UART1_ReceiveByte(void);
// uint8_t UART1_ReceiveString(char* str);

#endif