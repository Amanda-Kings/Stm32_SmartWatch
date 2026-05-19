#ifndef APP_MENU_H
#define APP_MENU_H

#include "stm32f10x_conf.h"

void Menu_Init(void);
void Peripheral_Init(void);
void Show_Clock_UI(void);
int First_Page_Clock(void);
int SettingPage(void);
int Menu(void);
void StopWatch_Tick(void);
int StopWatch(void);
int LED(void);
int MPU6050(void);
int Game(void);
int Emoji(void);
int Gradienter(void);

#endif /* APP_MENU_H */