#include "app_menu.h"
#include "bsp_oled.h"
#include "bsp_rtc.h"
#include "bsp_uart.h"
#include <stdio.h>

DateTime RTC_Time = {0};

void Show_Clock_UI(void){
    // Show_Battery();
	RTC_GetTimeStamp(&RTC_Time);
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",RTC_Time.year,RTC_Time.month,RTC_Time.day);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",RTC_Time.hour,RTC_Time.minute,RTC_Time.second);
	OLED_ShowString(0,48,"菜单",OLED_8X16);
	OLED_ShowString(96,48,"设置",OLED_8X16);
	OLED_Update();
}
int First_Page_Clock(void)
{
	
}
