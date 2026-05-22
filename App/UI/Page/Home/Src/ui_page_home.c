#include "ui_page_home.h"
#include "bsp_rtc.h"


static UI_Page page_home;

static void Page_Home_Draw(UI_Page *self);
static void Page_Home_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_Home_OnEnter(UI_Page *self);

// 主页虚函数表
static const UI_Page_VTable home_vtable = {
    .on_enter = Page_Home_OnEnter,
    .draw = Page_Home_Draw,
    .on_key_event = Page_Home_key
};
// 菜单项定义
static const UI_MenuItem menu_items[] = 
{
    { 
        .text = "菜单",
        .font_size = OLED_8X16,
        .x = 0,
        .y = 48,
        .width = 32,
        .height = 16,
        .target_page = UI_PAGE_MAX,
        .on_select = NULL
    },  // 跳转，无自定义动作
    { 
        .text = "设置",
        .font_size = OLED_8X16,
        .x = 96,
        .y = 48,
        .width = 32,
        .height = 16,
        .target_page = UI_PAGE_SETTING,
        .on_select = NULL
    }   // 不跳转
};

static void Page_Home_Draw(UI_Page *self)
{
    DateTime time = RTC_GetTimeStamp();
    // 绘制主页内容
    OLED_Clear();
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",time.year,time.month,time.day);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",time.hour,time.minute,time.second);
    for(uint8_t i = 0; i < self->item_count; i++)
    {
        OLED_ShowString(self->items[i].x,self->items[i].y,self->items[i].text,self->items[i].font_size);
    }
     // 绘制焦点反显
    OLED_ReverseArea(self->items[self->focus_index].x,
                     self->items[self->focus_index].y,
                     self->items[self->focus_index].width,
                     self->items[self->focus_index].height);
	OLED_Update();
}
static void Page_Home_key(UI_Page *self, KeyEventInfo_TypeDef *key)
{
    if (key->event != KEY_EVENT_SHORT) return;

    switch(key->type)
    {
        case KEY_UP:
            self->focus_index = (self->focus_index + self->item_count - 1) % self->item_count;
            Page_Home_Draw(self);  // 重绘整个页面
            break;
            
        case KEY_DOWN:
            self->focus_index = (self->focus_index + 1) % self->item_count;
            Page_Home_Draw(self);
            break;
            
        case KEY_ENTER: 
            UI_Page_TypeDef target = self->items[self->focus_index].target_page;
            if (target != UI_PAGE_MAX) 
            {
                UI_SwitchPage(target);
            } else if (self->items[self->focus_index].on_select) 
            {
                self->items[self->focus_index].on_select(self);
            }
            break;
        
        default:
            break;
    }
}

static void Page_Home_OnEnter(UI_Page *self) {
    Page_Home_Draw(self);
}

void Page_Home_Init(void) {
    page_home.page_state = UI_PAGE_HOME;
    page_home.vtable = &home_vtable;
    page_home.focus_index = 0;
    page_home.items = menu_items;
    page_home.item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    page_home.refresh_interval = 0;
    page_home.last_refresh = 0;
    UI_RegisterPage(&page_home);
}