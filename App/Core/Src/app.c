#include "app.h"
#include "bsp_key.h"
#include "ui_core.h"
#include "ui_page_home.h"
#include "ui_page_setting.h"
#include "ui_page_setting_datetime.h"
#include "ui_page_menu.h"
#include "bsp_sys_tick.h"

void App_Init(void) {
    UI_Init();
    
    Key_RegisterCallback(KEY_UP,    UI_HandleKeyEvent);
    Key_RegisterCallback(KEY_DOWN,  UI_HandleKeyEvent);
    Key_RegisterCallback(KEY_ENTER, UI_HandleKeyEvent);

    Page_Home_Init();                          // 注册首页
    SetPageRefreshInterval(UI_PAGE_HOME, 500); // 首页刷新间隔500ms
    UI_SwitchPage(UI_PAGE_HOME);               // 默认切换到首页

    Page_Setting_Init();
    SetPageRefreshInterval(UI_PAGE_SETTING, 0); // 关闭刷新


    Page_SetDateTime_Init();
    SetPageRefreshInterval(UI_PAGE_SETTING_DATETIME, 0);

    Page_Menu_Init();
    SetPageRefreshInterval(UI_PAGE_MENU, 0); 
}

static void App_KeyEventTask(void) {
    uint16_t max_events = 16;
    KeyEventInfo_TypeDef evt;
    while (max_events-- && Key_QueuePop(&evt)) {
        callbackItemRun(&evt);   // 此时安全，可绘图
    }
}
void App_Task(void)
{
    App_KeyEventTask();
    UI_Refresh();                 // 页面刷新
}