#ifndef BSP_KEY_H
#define BSP_KEY_H

#include "stm32f10x_conf.h"

// 按键类型
typedef enum
{
    KEY_UP = 0,
    KEY_DOWN,
    KEY_ENTER,
    KEY_TYPE_MAX
} Key_TypeDef;

// 按键事件类型
typedef enum
{
    KEY_EVENT_NONE = 0, // 无事件
    KEY_EVENT_SHORT,    // 短按
    KEY_EVENT_LONG,     // 长按
    KEY_EVENT_REPEAT,   // 连发
    KEY_EVENT_RELEASE,  // 释放
} KeyEvent_TypeDef;

// 1. 按键事件信息结构体
typedef struct {
    Key_TypeDef type;
    KeyEvent_TypeDef event;
} KeyEventInfo_TypeDef;

// 2. 回调函数类型
typedef void (*KeyCallback_t)(KeyEventInfo_TypeDef *info);

// 3. 按键处理函数
void Key_Init(void);
void Key_RegisterCallback(Key_TypeDef type, KeyCallback_t callback);
uint8_t Key_QueuePop(KeyEventInfo_TypeDef *info);
void callbackItemRun(KeyEventInfo_TypeDef *info);
void Key_Scan(void);
void Key_Scan_Tick(void);

#endif
