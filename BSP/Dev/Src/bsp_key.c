#include "bsp_key.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x.h"
#include <stdint.h>

// 读取按键电平（有效电平转换为 1，无效为 0)
static uint8_t Key_ReadLevel(KeyObject_TypeDef *key)
{
    return GPIO_ReadInputDataBit(key->hw.GPIOx, key->hw.GPIO_Pin);
}

void Key_Init(KeyObject_TypeDef *key)
{
    key->type = KEY_MAX; // 默认未定义类型
    key->state = KEY_IDLE;
    key->event = KEY_NONE;
    key->cnt = 0;
    key->repeat_cnt = 0;
    key->is_long_press = 0;

    RCC_APB2PeriphClockCmd(key->hw.RCC_APB2Periph, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = key->hw.GPIO_Pin;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; // 上拉输入
    GPIO_Init(key->hw.GPIOx, &GPIO_InitStructure);
}
void Key_Scan(KeyObject_TypeDef *key)
{
    uint8_t cur_level = Key_ReadLevel(key);
    key->event = KEY_NONE; // 每次扫描清空事件

    switch (key->state)
    {
    // 空闲状态
    case KEY_IDLE:
        if (cur_level == key->hw.Active_Level)
        {
            key->state = KEY_DEBOUNCE;
            key->cnt = 0;
        }
        break;
    // 按下消抖
    case KEY_DEBOUNCE:
        key->cnt++;
        if (key->cnt >= key->debounce_time)
        {
            if (cur_level == key->hw.Active_Level)
            {
                key->state = KEY_PRESS_DOWN; // 消抖完成，确认按下
                key->cnt = 0;                // 清零，准备计时长按
                key->repeat_cnt = 0;         // 连发计数器清零
            }
            else
            {
                key->state = KEY_IDLE; // 抖动，退回空闲
            }
            key->cnt = 0; // 重置计数器
        }
        break;
    // 按下确认（等待长按/释放）
    case KEY_PRESS_DOWN:
        key->cnt++;
        // 长按条件满足
        if (key->cnt >= key->long_time)
        {
            key->state = KEY_LONG_DETECT; // 进入长按检测状态
            key->event = KEY_LONG_PRESS;  // 触发一次长按事件
            key->cnt = 0;                 // 重置计数器
            key->repeat_cnt = 0;          // 连发计数器清零
            key->is_long_press = 1;       // 标记为长按
        }
        // 检测按键释放
        else if (cur_level != key->hw.Active_Level)
        {
            key->state = KEY_RELEASE_DEBOUNCE; // 进入释放消抖状态
            key->cnt = 0;                      // 重置计数器
            key->is_long_press = 0;            // 标记为短按
        }
        break;
    // 长按检测（连发/等待释放）
    case KEY_LONG_DETECT:
        // 按键仍按下，处理连发
        if (cur_level == key->hw.Active_Level)
        {
            if (key->repeat_time > 0) // repeat_time = 0 表示禁用连发
            {
                key->repeat_cnt++;
                if (key->repeat_cnt >= key->repeat_time)
                {
                    key->event = KEY_CONTINUE_PRESS; // 触发连发事件
                    key->repeat_cnt = 0;             // 重置连发计数器
                }
            }
        }
        // 检测按键释放
        else
        {
            key->state = KEY_RELEASE_DEBOUNCE; // 进入释放消抖状态
            key->cnt = 0;
        }
        break;
    // 释放消抖
    case KEY_RELEASE_DEBOUNCE:
        key->cnt++;
        if (key->cnt >= key->debounce_time)
        {
            if (cur_level != key->hw.Active_Level)
            {
                // 释放确认，根据标记产生事件
                key->event = key->is_long_press ? KEY_RELEASE : KEY_SHORT_PRESS;
            }
            // 不论电平如何，结束本次按键过程
            key->state = KEY_IDLE;
            key->cnt = 0;
            key->repeat_cnt = 0;
        }
        // 如果在释放消抖过程中又检测到按下，可能是抖动或再次按下，退回长按检测状态
        else if (cur_level == key->hw.Active_Level)
        {
            if (key->is_long_press) // 只有之前是长按才退回长按检测状态
            {
                key->state = KEY_LONG_DETECT;
                key->cnt = 0; // 重置计数器，继续长按检测
            }
            else
            {
                key->state = KEY_PRESS_DOWN; // 之前是短按，退回按下确认状态
            }
            key->cnt = 0; // 重置计数器，继续等待长按或释放
        }
        break;
    default:
        key->state = KEY_IDLE;
        break;
    }
    // 当 key->event 被赋值后，调用回调
    if (key->event != KEY_NONE && key->callback != NULL)
    {
        key->callback(key);
    }
}