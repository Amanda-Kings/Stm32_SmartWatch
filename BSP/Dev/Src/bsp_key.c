#include "bsp_key.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include <stdint.h>

#define DEBOUNCE_TIME 2 // 20ms消抖
#define LONG_PRESS_TIME 50 // 500ms长按
#define REPEAT_TIME 20 // 200ms连发

// 按键状态类型
typedef enum
{
    KEY_STATE_IDLE = 0,        // 未按下
    KEY_STATE_DEBOUNCE_PRESS,  // 按下消抖
    KEY_STATE_PRESSED,         // 按下
    KEY_STATE_LONG_DETECT,     // 长按检测
    KEY_STATE_DEBOUNCE_RELEASE // 释放消抖
} KeyState_TypeDef;


// 按键硬件配置结构体
typedef struct
{
    uint32_t RCC_APB2Periph; // 按键对应的RCC_APB2Periph时钟
    GPIO_TypeDef *GPIOx;     // 按键对应的GPIO端口
    uint16_t GPIO_Pin;       // 按键对应的GPIO引脚
    uint8_t Active_Level;    // 有效电平（0 = 低电平有效，1 = 高电平有效）
} KeyHW_ConfigTypeDef;
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
}KeyObject_TypeDef;

// 按键回调函数表
static KeyCallback_t callback_table[KEY_TYPE_MAX];

 //（扫描周期为10ms）20ms消抖, 500ms长按，200ms连发
// #define KEY_DEF(id, port, pin) \
//     [id] = { \
//         .type = id, \
//         .hw = { RCC_APB2Periph_##port, port, pin, 0 }, \
//         .debounce_time = 2, \
//         .long_time = 50, \
//         .repeat_time = 20, \
//         .state = KEY_STATE_IDLE \
//     }

static KeyObject_TypeDef keyObjects[] = 
{
    [KEY_UP] = {
        .type = KEY_UP,
        .hw = {
            .RCC_APB2Periph = RCC_APB2Periph_GPIOB,
            .GPIOx = GPIOB,
            .GPIO_Pin = GPIO_Pin_1,
            .Active_Level = 0
        },
        .debounce_time = DEBOUNCE_TIME,
        .long_time = LONG_PRESS_TIME,
        .repeat_time = REPEAT_TIME,
        .state = KEY_STATE_IDLE  // 初始状态
    },
    [KEY_DOWN] = {
        .type = KEY_DOWN,
        .hw = {
            .RCC_APB2Periph = RCC_APB2Periph_GPIOA,
            .GPIOx = GPIOA,
            .GPIO_Pin = GPIO_Pin_6,
            .Active_Level = 0
        },
        .debounce_time = DEBOUNCE_TIME,
        .long_time = LONG_PRESS_TIME,
        .repeat_time = REPEAT_TIME,
        .state = KEY_STATE_IDLE
    },
    [KEY_ENTER] = {
        .type = KEY_ENTER,
        .hw = {
            .RCC_APB2Periph = RCC_APB2Periph_GPIOA,
            .GPIOx = GPIOA,
            .GPIO_Pin = GPIO_Pin_4,
            .Active_Level = 0
        },
        .debounce_time = DEBOUNCE_TIME,
        .long_time = LONG_PRESS_TIME,
        .repeat_time = REPEAT_TIME,
        .state = KEY_STATE_IDLE
    }
};

#define KEY_COUNT (sizeof(keyObjects) / sizeof(keyObjects[0]))

static void NotifyEvent(KeyObject_TypeDef *key, KeyEvent_TypeDef event) {
    if (callback_table[key->type]) {
        KeyEventInfo_TypeDef info = {
            .type = key->type,
            .event = event
        };
        callback_table[key->type](&info);
    }
}
static void ProcessKey (KeyObject_TypeDef *key, uint8_t level)
{
    uint8_t is_active = (level == key->hw.Active_Level);
    key->event = KEY_EVENT_NONE; // 每次扫描清空事件

    switch (key->state)
    {
        // 空闲状态
        case KEY_STATE_IDLE:
            if (is_active)
            {
                key->state = KEY_STATE_DEBOUNCE_PRESS;
                key->cnt = 0;
            }
            break;
        // 按下消抖状态
        case KEY_STATE_DEBOUNCE_PRESS:
            key->cnt++;
            if (key->cnt >= key->debounce_time)
            {
                if (is_active)
                {
                    key->state = KEY_STATE_PRESSED;     // 消抖完成，进入按下状态
                    key->cnt = 0;                       // 重置计数器
                    key->is_long_press = 0;             // 默认不是长按
                    NotifyEvent(key, KEY_EVENT_SHORT);  // 触发短按事件
                }
                else
                {
                    key->state = KEY_STATE_IDLE;        // 抖动，退回空闲
                    key->cnt = 0;                       // 重置计数器
                }
                
            }
            break;
        // 按下状态
        case KEY_STATE_PRESSED:
            key->cnt++;
            if (key->cnt >= key->long_time)
            {
                key->state = KEY_STATE_LONG_DETECT;     // 进入长按检测状态
                key->cnt = 0;                           // 重置计数器
                key->repeat_cnt = 0;                    // 连发计数器清零
                key->is_long_press = 1;                 // 标记为长按
                NotifyEvent(key, KEY_EVENT_LONG);       // 触发长按事件
            }
            // 检测按键释放
            else if (!is_active)
            {
                key->state = KEY_STATE_DEBOUNCE_RELEASE;  // 进入释放消抖状态
                key->cnt = 0;                             // 重置计数器
            }
            break;
        // 长按检测
        case KEY_STATE_LONG_DETECT:
            // 按键仍按下，处理连发
            if (is_active)
            {
                key->repeat_cnt++;
                if (key->repeat_time > 0 && key->repeat_cnt >= key->repeat_time) 
                {
                    key->repeat_cnt = 0;                // 重置连发计数器
                    NotifyEvent(key, KEY_EVENT_REPEAT); // 触发连发事件
                }
            }
            // 检测按键释放
            else
            {
                key->state = KEY_STATE_DEBOUNCE_RELEASE; // 进入释放消抖状态
                key->cnt = 0;
            }
            break;
        // 释放消抖状态
        case KEY_STATE_DEBOUNCE_RELEASE:
            key->cnt++;
            if (key->cnt >= key->debounce_time)
            {
                if (!is_active)
                {
                    // 消抖完成，确认释放
                    key->state = KEY_STATE_IDLE;
                    key->cnt = 0;
                    NotifyEvent(key, KEY_EVENT_RELEASE);
                }
                else
                {
                    // 抖动，退回之前的状态
                    key->state = (key->is_long_press) ? KEY_STATE_LONG_DETECT : KEY_STATE_PRESSED;
                    key->cnt = 0;
                }
            }
            break;
        default:
            key->state = KEY_STATE_IDLE;
            break;
    }
}

void Key_Init(void) 
{
    for (int i = 0; i < KEY_COUNT; i++) {
        RCC_APB2PeriphClockCmd(keyObjects[i].hw.RCC_APB2Periph, ENABLE);
        GPIO_InitTypeDef GPIO_InitStruct = {
            .GPIO_Pin = keyObjects[i].hw.GPIO_Pin,
            .GPIO_Mode = GPIO_Mode_IPU,
            .GPIO_Speed = GPIO_Speed_50MHz
        };
        GPIO_Init(keyObjects[i].hw.GPIOx, &GPIO_InitStruct);
    }
}

void Key_RegisterCallback(Key_TypeDef type, KeyCallback_t callback) 
{
    if (type < KEY_TYPE_MAX) {
        callback_table[type] = callback;
    }
}

void Key_Scan(void) 
{
    for (int i = 0; i < KEY_COUNT; i++) {
        uint8_t level = GPIO_ReadInputDataBit(
            keyObjects[i].hw.GPIOx,
            keyObjects[i].hw.GPIO_Pin
        );
        ProcessKey(&keyObjects[i], level);
    }
}