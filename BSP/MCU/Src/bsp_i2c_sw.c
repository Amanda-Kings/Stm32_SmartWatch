#include "bsp_i2c_sw.h"
#include "delay.h"

void I2C_SW_Init(void){
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
}
void I2C_SW_Start(void)
{
    SCL_HIGH;
    SDA_HIGH;
    I2C_DELAY;
    SDA_LOW;
    I2C_DELAY;
}
void I2C_SW_Stop(void)
{
    SCL_HIGH;
    SDA_LOW;
    I2C_DELAY;
    SDA_HIGH;
    I2C_DELAY;
}
void I2C_SW_Ack(void){
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;

    SDA_LOW;
    I2C_DELAY;

    SCL_HIGH;
    I2C_DELAY;

    SCL_LOW;
    I2C_DELAY;

    SDA_HIGH;
    I2C_DELAY;
}
void I2C_SW_Nack(void){ 
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;

    SCL_HIGH;
    I2C_DELAY;

    SCL_LOW;
    I2C_DELAY;
}

uint8_t I2C_SW_WaitAck(void)
{
    SCL_LOW;
    SDA_HIGH;
    I2C_DELAY;

    SCL_HIGH;
    I2C_DELAY;

    uint16_t ack = READ_SDA;

    SCL_LOW;
    I2C_DELAY;

    return ack ? NACK : ACK;
}

void I2C_SW_SendByte(uint8_t byte)
{
   for(uint8_t i = 0; i < 8; i++)
   {
        SCL_LOW;
        SDA_LOW;
        I2C_DELAY;

        if(byte & 0x80)
        {
            SDA_HIGH;
        }
        else
        {
            SDA_LOW;
        }
        I2C_DELAY;

        SCL_HIGH;
        I2C_DELAY;

        SCL_LOW;
        I2C_DELAY;

        byte <<= 1;
   }
}
uint8_t I2C_SW_ReadByte(void)
{
    uint8_t data = 0;

    for(uint8_t i = 0; i < 8; i++)
    {
        SCL_LOW;
        I2C_DELAY;

        SCL_HIGH;
        I2C_DELAY;

        data <<= 1;
        if (READ_SDA)
        {
            data |= 0x01;
        }

        SCL_LOW;
        I2C_DELAY;
    }

    return data;
}
