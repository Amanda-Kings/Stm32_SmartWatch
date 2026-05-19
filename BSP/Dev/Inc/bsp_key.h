#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "stm32f10x_conf.h"

// 按键类型
typedef enum
{
    KEY_UP = 0,
    KEY_DOWN,
    KEY_ENTER,
    KEY_MAX
} Key_TypeDef;

// 按键事件类型
typedef enum
{
    KEY_NONE = 0,       // 无事件
    KEY_SHORT_PRESS,    // 短按
    KEY_LONG_PRESS,     // 长按
    KEY_CONTINUE_PRESS, // 连续按
    KEY_RELEASE         // 按键释放
} KeyEvent_TypeDef;

// 按键状态类型
typedef enum
{
    KEY_IDLE = 0,        // 空闲：未按下
    KEY_DEBOUNCE,        // 按下消抖
    KEY_PRESS_DOWN,      // 按下确认
    KEY_LONG_DETECT,     // 长按检测
    KEY_RELEASE_DEBOUNCE // 释放消抖
} KeyState_TypeDef;

// 按键回调函数类型

// 按键硬件配置结构体
typedef struct
{
    uint32_t RCC_APB2Periph; // 按键对应的RCC_APB2Periph时钟
    GPIO_TypeDef *GPIOx;     // 按键对应的GPIO端口
    uint16_t GPIO_Pin;       // 按键对应的GPIO引脚
    uint8_t Active_Level;    // 有效电平（0 = 低电平有效，1 = 高电平有效）
} __attribute__((packed)) KeyHW_ConfigTypeDef;
// 按键对象结构体
typedef struct
{
    Key_TypeDef type;       // 按键类型
    KeyHW_ConfigTypeDef hw; // 按键硬件配置

    KeyState_TypeDef state; // 按键状态
    KeyEvent_TypeDef event; // 按键事件

    uint16_t cnt;          // 按键计数
    uint16_t repeat_cnt;   // 连发间隔计数器
    uint8_t is_long_press; // 标记此次是长按（1）还是短按（0）

    uint16_t debounce_time; // 按键消抖时间
    uint16_t long_time;     // 按键长按时间
    uint16_t repeat_time;   // 连发间隔（扫描次数），0 表示不连发

    void (*callback)(KeyObject_TypeDef *key); // 按键回调函数
} __attribute__((aligned(64))) KeyObject_TypeDef;

void Key_Init(KeyObject_TypeDef *key);
void Key_Scan(KeyObject_TypeDef *key);

#endif
