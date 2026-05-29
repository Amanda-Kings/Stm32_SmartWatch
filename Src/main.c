#include <stdio.h>
#include "bsp_init.h"
#include "app.h"
#include "delay.h"
#include "test.h"

// #define TEST_DeBUG

#ifdef TEST_DeBUG
int main(void){    
    BSP_Init();         //初始化底层外设
    Test_OLED_Update_Speed();
    // Test_Run();
    while (1)
    {
        
    }
}
#else
int main(void){    
    BSP_Init();         //初始化底层外设
    App_Init();         //初始化应用层
    while (1)
    {
        App_Task();     //应用层任务
    }
}

#endif