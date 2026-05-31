#include "ui_page_menu.h"
#include "slide_menu.h"
#include <stdio.h>

// 外框固定位置
#define FRAME_X  42
#define FRAME_Y  10
#define FRAME_W  44
#define FRAME_H  44
#define MENU_COUNT 7
#define SCREEN_WIDTH 128

#define ANIM_INTERVAL_MS  30       // 约33fps
static uint32_t last_anim_tick = 0;

static UI_Page page_meun;
static SlideMenu menu;
/* ========== 静态函数声明 ========== */
static void Page_Meun_Draw(UI_Page *self);
static void Page_Meun_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_Meun_OnEnter(UI_Page *self);



static const UI_Page_VTable Meun_vtable = {
    .on_enter = Page_Meun_OnEnter,
    .draw = Page_Meun_Draw,
    .on_key_event = Page_Meun_key
};
void Page_Menu_Init(void)
{
    page_meun.page_state = UI_PAGE_MENU;
    page_meun.vtable = &Meun_vtable;
    page_meun.refresh_interval = 0;
    page_meun.last_refresh = 0;
    page_meun.items = NULL;   // 或一个空数组
    page_meun.item_count = 0;
    page_meun.focus_index = 0;
    page_meun.data = NULL;
    UI_RegisterPage(&page_meun);

    SlideMenu_Init(&menu, MENU_COUNT, SCREEN_WIDTH);
}
static void Page_Meun_OnEnter(UI_Page *self)
{
    Page_Meun_Draw(self);
}
static void DrawIcon(uint8_t index, int16_t x, int16_t y) {
    OLED_ShowImage(x, y, ICON_WIDTH, ICON_HEIGHT, Menu_Graph[index]);
}
static void Page_Meun_Draw(UI_Page *self)
{
     // 1. 推进动画（框架保证了调用间隔 >= refresh_interval）
    SlideMenu_Update(&menu);

    // 2. 动画是否结束？
    if (!menu.is_animating) {
        self->refresh_interval = 0;      // 关闭自动刷新
    }

    // 4. 真正刷新屏幕
    OLED_Clear();
    OLED_ShowImage(FRAME_X, FRAME_Y, FRAME_W, FRAME_H, Frame);
    SlideMenu_Draw(&menu, DrawIcon);
    OLED_Update();
}

static UI_Page_TypeDef SwitchTarget(int8_t index)
{
    UI_Page_TypeDef target = UI_PAGE_MAX;
    switch (index)
    {
        case 0:
            target=UI_PAGE_HOME;
            break;
        case 1:
            target=UI_PAGE_STOPWATCH;
            break;
        case 2:
            target=UI_PAGE_FLASHLIGHT;
            break;
        case 3:
            target=UI_PAGE_HOME;
            break;
        case 4:
            target=UI_PAGE_HOME;
            break;
        case 5:
            target=UI_PAGE_HOME;
            break;
        case 6:
            target=UI_PAGE_HOME;
            break;
        case 7:
            target=UI_PAGE_HOME;
            break;
        default:
            target = UI_PAGE_MAX;
            break;
    }
    return target;
}

static void Page_Meun_key(UI_Page *self, KeyEventInfo_TypeDef *key)
{  
    if (key->event != KEY_EVENT_SHORT) return;
    switch (key->type)
    {
        case KEY_UP:
                if (SlideMenu_Move(&menu, 1)) 
                {
                    self->refresh_interval = 16;
                    Page_Meun_Draw(self);                      // 立即绘制第一帧
                }
            break;
        case KEY_DOWN:
                if (SlideMenu_Move(&menu, 0)) 
                {
                    self->refresh_interval = 16;
                    Page_Meun_Draw(self);                      // 立即绘制第一帧
                }
            break;
        case KEY_ENTER:
            int8_t idx = SlideMenu_GetSelectedIndex(&menu);
                if (idx >= 0) {
                    // 执行选中项的功能
                    UI_SwitchPage(SwitchTarget(idx));
                }
            break;
        default:
            break;
    }
}
