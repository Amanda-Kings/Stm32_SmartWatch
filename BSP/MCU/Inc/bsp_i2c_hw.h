#ifndef BSP_I2C_HW_H
#define BSP_I2C_HW_H

#include "stm32f10x_conf.h"

void I2C_HW_Init(I2C_TypeDef *MyI2C);
void I2C_HW_SendByte(I2C_TypeDef *MyI2C, uint8_t dev_addr, uint8_t reg_addr, const uint8_t data);
void I2C_HW_SendString(I2C_TypeDef *MyI2C, const uint8_t* str, uint32_t len);
uint8_t I2C_HW_ReceiveByte(I2C_TypeDef *MyI2C, uint8_t dev_addr, uint8_t reg_addr);
uint8_t I2C_HW_ReceiveString(I2C_TypeDef *MyI2C, uint8_t dev_addr, uint8_t reg_addr, uint8_t* str, uint32_t len);

#endif