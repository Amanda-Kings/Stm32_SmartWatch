#include "bsp_i2c_hw.h"

void I2C1_HD_Init(void)
{
    // I2C initialization code here
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    


}
void I2C1_HD_SendByte(uint8_t byte);
void I2C1_HD_SendString(const char* str, uint32_t len);
uint8_t I2C_HD_ReceiveByte(void);
uint8_t I2C_HD_ReceiveString(char* str, uint32_t len);

