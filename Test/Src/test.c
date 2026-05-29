#include "test.h"
#include <stdio.h>
#include "bsp_uart.h"
#include "bsp_key.h"
#include "bsp_sys_tick.h"
#include "bsp_oled.h"
void Test_Init(void)
{
    UART1_Init();
    Key_Init();
}


void TestKey_Callback(KeyEventInfo_TypeDef *key)
{
    switch(key->event)
    {
        case KEY_EVENT_SHORT:
            printf("KEY_EVENT_SHORT\r\n");
            break;
        case KEY_EVENT_LONG:
            printf("KEY_EVENT_LONG\r\n");
            break;
        case KEY_EVENT_REPEAT:
            printf("KEY_EVENT_REPEAT\r\n");
            break;
        case KEY_EVENT_RELEASE:
            printf("KEY_EVENT_RELEASE\r\n");
            break;
        default:
            break;
    }
}

static void Test_KeyScanTask(uint16_t delay) {
    static uint32_t last_tick = 0;
    if(SysTick_GetTick() - last_tick >= delay)
    {
        last_tick = SysTick_GetTick();
        Key_Scan();
    }
}

static void Test_Key(void)
{
    Key_RegisterCallback(KEY_DOWN, TestKey_Callback);
    Key_RegisterCallback(KEY_UP, TestKey_Callback);
    Key_RegisterCallback(KEY_ENTER, TestKey_Callback);

    printf("Test_Key......\r\n");

    while(1)
    {
        Test_KeyScanTask(10);
    }
}
void Test_Run(void)
{
    Test_Key();
}

void Test_OLED_Update_Speed(void) {
    // 1. 先绘制简单内容，模拟真实操作（可选）
    OLED_Clear();                  // 清空显存
    OLED_ShowString(0, 0, "Test", 8);  // 随便写点东西
    // 此时显存已更新，但屏幕尚未刷新

    // 2. 测量 OLED_Update 耗时
    uint32_t start = SysTick_GetTick();
    OLED_Update();                 // 将显存发送到屏幕
    uint32_t elapsed = SysTick_GetTick() - start;

    printf("OLED_Update 耗时: %lu ms\r\n", elapsed);
}
