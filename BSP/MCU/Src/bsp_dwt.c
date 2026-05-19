#include "bsp_dwt.h"
void DWT_Init(void){
    // 使能DWT
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk; // 使能DWT
    DWT->CYCCNT = 0; // 清零计数器
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk; // 使能CYCCNT
}