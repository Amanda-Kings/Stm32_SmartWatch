#ifndef BSP_MYRTC_H
#define BSP_MYRTC_H

#include "stm32f10x_conf.h"
#include <time.h>

// RTC初始时间
#define RTC_StartTime_YEAR 2020
#define RTC_StartTime_MONTH 1
#define RTC_StartTime_DAY 1
#define RTC_StartTime_HOUR 0
#define RTC_StartTime_MINUTE 0
#define RTC_StartTime_SECOND 0

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
} DateTime;


void RTC_Init(void);
void RTC_SetTimeStamp(const DateTime *dataTime);
void RTC_GetTimeStamp(DateTime *dataTime);

#endif
