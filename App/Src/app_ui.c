#include "app_ui.h"
#include "bsp_oled.h"


static UI_Page_State current_page = UI_PAGE_HOME; // 全局：当前UI状态
static uint8_t current_cursor;                    // 当前光标选中项
void UI_Init(void)
{
    current_page = UI_PAGE_HOME;
    current_cursor = ;
}
// ===================== 主页面 =====================
static void UI_MainPage(void)
{
    // 刷新界面内容
}
static void UI_MainPage_Key(KeyEvent_TypeDef e)
{
    if(e == KEY_SHORT_PRESS)
    {
        current_page = UI_PAGE_SETTING; // 切换到设置
    }
}

// ===================== 设置页面 =====================
static void UI_SettingPage(void) {}
static void UI_SettingPage_Key(KeyEvent_TypeDef e)
{
    if(e == KEY_SHORT_PRESS)
    {
        current_page = UI_PAGE_HOME; // 返回主页
    }
}

void UI_Task(void)
{
    switch (current_page)
    {
    case UI_PAGE_HOME:
        // 显示主页面
        break;
    case UI_PAGE_SETTING:
        // 显示设置页面
        break;
    case UI_PAGE_MENU:
        // 显示菜单页面
        break;
    default:
        break;
    }
}