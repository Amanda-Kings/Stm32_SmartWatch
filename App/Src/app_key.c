#include "app_key.h"
#include "bsp_key.h"
#include "bsp_sys_tick.h"
#include "bsp_oled.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "ui_core.h"
#include <stdio.h>
#include <stdint.h>

static KeyObject_TypeDef KeyUp = {
    .type = KEY_UP, // 上一页
    .hw = {
        .RCC_APB2Periph = RCC_APB2Periph_GPIOB,
        .GPIOx = GPIOB,
        .GPIO_Pin = GPIO_Pin_1,
        .Active_Level = 0 // 0 = 低电平有效
    },
    .debounce_time = 2, // 20ms消抖
    .long_time = 50,    // 500ms长按
    .repeat_time = 20,  // 200ms连发
    .callback = UI_OnKeyEvent
};
static KeyObject_TypeDef KeyDown = {
    .type = KEY_DOWN, // 下一页
    .hw = {
        .RCC_APB2Periph = RCC_APB2Periph_GPIOA,
        .GPIOx = GPIOA,
        .GPIO_Pin = GPIO_Pin_6,
        .Active_Level = 0 // 0 = 低电平有效
    },
    .debounce_time = 2, // 20ms消抖
    .long_time = 50,    // 500ms长按
    .repeat_time = 20,   // 200ms连发
    .callback = UI_OnKeyEvent
};
static KeyObject_TypeDef KeyEnter = 
{
    .type = KEY_ENTER, // 确认
    .hw = {
        .RCC_APB2Periph = RCC_APB2Periph_GPIOA,
        .GPIOx = GPIOA,
        .GPIO_Pin = GPIO_Pin_4,
        .Active_Level = 0 // 0 = 低电平有效
    },
    .debounce_time = 2, // 20ms消抖
    .long_time = 50,    // 500ms长按
    .repeat_time = 20,  // 200ms连发
    .callback = UI_OnKeyEvent
};

static void KeyUp_Callback(KeyEvent_TypeDef event)
{
    // 根据事件类型执行相应操作
    switch (event)
    {
    case KEY_SHORT_PRESS:
        // 处理KeyUp短按事件
        printf("KeyUp Short Press\r\n");
        OLED_ReverseArea(0, 0, 128, 16); // 反转OLED第一行显示
        break;
    case KEY_LONG_PRESS:
        // 处理KeyUp长按事件
        printf("KeyUp Long Press\r\n");
        break;
    case KEY_CONTINUE_PRESS:
        // 处理KeyUp连发事件
        printf("KeyUp Continue Press\r\n");
        break;
    case KEY_RELEASE:
        // 处理KeyUp释放事件
        printf("KeyUp Release\r\n");
        break;
    default:
        break;
    }
}

void App_KeyInit(void)
{
    Key_Init(&KeyUp);
    Key_Init(&KeyDown);
    Key_Init(&KeyEnter);
}
void App_KeyScanTask(void)
{
    static uint32_t tick = 0;
    if (SysTick_GetTick() - tick >= 10)
    {
        tick = SysTick_GetTick();
        Key_Scan(&KeyUp);
        Key_Scan(&KeyDown);
        Key_Scan(&KeyEnter);
    }
}