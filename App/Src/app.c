#include "app.h"
#include "bsp_key.h"
#include "ui_core.h"
#include "ui_page_home.h"
#include "bsp_sys_tick.h"

void App_Init(void) {
    UI_Init();
    
    Key_RegisterCallback(KEY_UP,    UI_HandleKeyEvent);
    Key_RegisterCallback(KEY_DOWN,  UI_HandleKeyEvent);
    Key_RegisterCallback(KEY_ENTER, UI_HandleKeyEvent);

    Page_Home_Init();                          // 注册首页
    SetPageRefreshInterval(UI_PAGE_HOME, 500); // 首页刷新间隔500ms
    UI_SwitchPage(UI_PAGE_HOME);               // 默认切换到首页
}

static void App_KeyScanTask(uint16_t delay) {
    static uint32_t last_tick = 0;
    if(SysTick_GetTick() - last_tick >= delay)
    {
        last_tick = SysTick_GetTick();
        Key_Scan();
    }
}
void App_Task(void)
{
    App_KeyScanTask(10);          // 按键扫描(10ms)
    UI_Refresh();                 // 页面刷新
}