#include <stdio.h>
#include "bsp_init.h"
#include "app_menu.h"
#include "app_key.h"
#include "bsp_sys_tick.h"
#include "delay.h"

int main(void){
    BSP_Init();         //初始化底层外设
    App_KeyInit();      //初始化按键
    printf("System Initialized...\n");
    while (1)
    {
        // Delay_ms(998);
        // printf("Tick: %d\n", SysTick_GetTick());
        //扫描按键事件
        App_KeyScanTask();
        
        //显示时钟界面
        Show_Clock_UI();
    }
}
