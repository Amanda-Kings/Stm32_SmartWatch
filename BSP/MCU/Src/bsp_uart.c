#include "bsp_uart.h"
#include <stdio.h>
void UART1_Init(void)
{
    // UART initialization code here
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure1;
    GPIO_InitStructure1.GPIO_Pin = GPIO_Pin_9; // TX
    GPIO_InitStructure1.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_InitStructure1.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure1);

    GPIO_InitTypeDef GPIO_InitStructure2;
    GPIO_InitStructure2.GPIO_Pin = GPIO_Pin_10; // RX
    GPIO_InitStructure2.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure2);

    USART_InitTypeDef USART_InitStructure;
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStructure);
    USART_Cmd(USART1, ENABLE);
}
void UART1_SendByte(uint8_t byte)
{
    // Code to send a byte over UART1
    while((!USART_GetFlagStatus(USART1, USART_FLAG_TXE))&& (!USART_GetFlagStatus(USART1, USART_FLAG_TC)));
    USART_SendData(USART1, byte);
    
}
void UART1_SendString(const char* str, uint32_t len)
{
    // Code to send a string over UART1
    for(uint32_t i = 0; i < len; i++)
    {
        UART1_SendByte(str[i]);
    }
}

// printf 重定向到串口1
int fputc(int ch, FILE *f)
{
    UART1_SendByte((uint8_t)ch);  // 发送字符
    return ch;
}