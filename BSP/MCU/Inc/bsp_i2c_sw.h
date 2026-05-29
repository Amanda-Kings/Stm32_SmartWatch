#ifndef BSP_I2C_SW_H
#define BSP_I2C_SW_H

#include "stm32f10x_conf.h"
#include "delay.h"
enum
{
    ACK = 0,
    NACK = 1
};

#define SCL_HIGH (GPIOB->ODR |= GPIO_ODR_ODR8)
#define SCL_LOW (GPIOB->BRR |= GPIO_ODR_ODR8)
#define SDA_HIGH (GPIOB->ODR |= GPIO_ODR_ODR9)
#define SDA_LOW (GPIOB->BRR |= GPIO_ODR_ODR9)

#define READ_SDA (GPIOB->IDR & GPIO_IDR_IDR9)

#define I2C_DELAY  do { \
    uint32_t start = DWT->CYCCNT; \
    while ((DWT->CYCCNT - start) < 54); \
} while(0)

void I2C_SW_Init(void);
void I2C_SW_Start(void);
void I2C_SW_Stop(void);
uint8_t I2C_SW_WaitAck(void);
void I2C_SW_Ack(void);
void I2C_SW_Nack(void);
void I2C_SW_SendByte(uint8_t byte);
uint8_t I2C_SW_ReadByte(void);

#endif