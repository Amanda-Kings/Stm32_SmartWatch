#include "test.h"
#include <stdio.h>
#include "bsp_uart.h"
#include "bsp_key.h"
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
