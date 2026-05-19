#include "bsp_led.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include "stm32f10x_gpio.h"

void LED_Init(void)
{
    // Implementation for LED initialization
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_15); // Start with LED off
}

void LED_On(void)
{
    // Implementation for turning LED on
    GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

void LED_Off(void)
{
    // Implementation for turning LED off
    GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void LED_Toggle(void)
{
    // Implementation for toggling LED
    GPIOB->ODR ^= GPIO_Pin_15;
}
