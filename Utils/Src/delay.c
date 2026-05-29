#include "delay.h"

void Delay_us(uint16_t us){
    uint32_t ticks_start = DWT->CYCCNT; // 获取当前的时钟周期计数
    // 72MHz：1us = 72 个时钟周期
    uint32_t ticks_delay = us * 72;

    // 循环等待，硬件计数，绝对精准
    while ((DWT->CYCCNT - ticks_start) < ticks_delay);
}
void Delay_ms(uint16_t ms){
    while(ms--){
        Delay_us(1000);
    }
}
void Delay_s(uint16_t s){
    while(s--){
        Delay_ms(1000);
    }
}

// 延时指定个数的 CPU 时钟周期（72MHz 下，1 周期 ≈ 14ns）
void Delay_ticks(uint32_t ticks) {
    uint32_t start = DWT->CYCCNT;
    while ((DWT->CYCCNT - start) < ticks);
}