#include "bsp_rtc.h"

static const DateTime RTC_StartTime = {
    .year = RTC_StartTime_YEAR,
    .month = RTC_StartTime_MONTH,
    .day = RTC_StartTime_DAY,
    .hour = RTC_StartTime_HOUR,
    .minute = RTC_StartTime_MINUTE,
    .second = RTC_StartTime_SECOND
};

static uint32_t DateTimeToTimestamp(const DateTime *dataTime) 
{
    struct tm t = {0};
    
    t.tm_year = dataTime->year - 1900;   // tm_year 从 1900 算起
    t.tm_mon  = dataTime->month - 1;     // tm_mon 是 0-11
    t.tm_mday = dataTime->day;
    t.tm_hour = dataTime->hour;
    t.tm_min  = dataTime->minute;
    t.tm_sec  = dataTime->second;
    
    return (uint32_t)mktime(&t); // mktime 返回 time_t，通常是 64 位，但我们只取低 32 位，适用于 1970-01-01 到 2106-02-07 的日期范围
}
static DateTime TimestampToDateTime(uint32_t timestamp) 
{
    time_t t = (time_t)timestamp;  // 值拷贝，32位安全扩展到64位
    struct tm *ptm = localtime(&t);  // 类型匹配，安全

    DateTime dataTime = {0}; // 初始化结构体，确保所有字段都有定义的初始值
    dataTime.year = (uint16_t)(ptm->tm_year + 1900);
    dataTime.month = (uint8_t)(ptm->tm_mon + 1);
    dataTime.day = (uint8_t)(ptm->tm_mday);
    dataTime.hour = (uint8_t)(ptm->tm_hour);
    dataTime.minute = (uint8_t)(ptm->tm_min);
    dataTime.second = (uint8_t)(ptm->tm_sec);
    
    return dataTime; // 返回结构体，编译器会优化为寄存器传递
}

void RTC_Init(void){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);		//开启PWR的时钟（电源管理）
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP, ENABLE);		//开启BKP的时钟（后备域寄存器）

    PWR_BackupAccessCmd(ENABLE);                            //允许访问后备寄存器

    if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
    {
        RCC_LSEConfig(RCC_LSE_ON);                              // 开启外部低速晶振   
        while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET);    // 等待外部低速晶振稳定
        RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);                 // 选择外部低速晶振作为RTC时钟源
        RCC_RTCCLKCmd(ENABLE);                                   // 使能RTC时钟
        RTC_WaitForSynchro();                                    // 等待RTC寄存器同步
        RTC_WaitForLastTask();								     //等待上一次操作完成

        RTC_SetPrescaler(32767);                                 //设置RTC预分频值，RTC时钟频率为LSE频率除以（预分频值+1），这里设置为1Hz
        RTC_WaitForLastTask();                                   //等待上一次操作完成

        RTC_SetTimeStamp(&RTC_StartTime);                         //设置RTC时间，初始值为RTC_StartTime定义的时间
        BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);			//在备份寄存器写入自己规定的标志位，用于判断RTC是不是第一次执行配置
    }
    else
    {
        RTC_WaitForSynchro();								//等待同步
		RTC_WaitForLastTask();								//等待上一次操作完成
    }

}

void RTC_SetTimeStamp(const DateTime *dataTime){
    RTC_WaitForLastTask();
    RTC_SetCounter(DateTimeToTimestamp(dataTime));
    RTC_WaitForLastTask();
}

void RTC_GetTimeStamp(DateTime *dataTime){
    RTC_WaitForSynchro();
    uint32_t second = RTC_GetCounter();
    *dataTime = TimestampToDateTime(second);
}
