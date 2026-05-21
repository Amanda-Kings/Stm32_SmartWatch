#include "bsp_init.h"
#include "bsp_dwt.h"
#include "bsp_rtc.h"
#include "bsp_uart.h"
#include "bsp_oled.h"
#include "bsp_sys_tick.h"

void BSP_Init(void)
{
    DWT_Init();
    RTC_Init();
    UART1_Init();
    OLED_Init();  
    SysTick_Init();
    Key_Init();
}