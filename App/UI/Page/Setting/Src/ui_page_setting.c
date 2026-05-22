#include "ui_page_setting.h"


static UI_Page page_setting;

static void Page_Setting_Draw(UI_Page *self);
static void Page_Setting_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_Setting_OnEnter(UI_Page *self);

// 主页虚函数表
static const UI_Page_VTable setting_vtable = {
    .on_enter = Page_Setting_OnEnter,
    .draw = Page_Setting_Draw,
    .on_key_event = Page_Setting_key
};

// 菜单项定义
static const UI_MenuItem menu_items[] = 
{
    { 
        .text = Return,
        .x = 0,
        .y = 0,
        .width = 16,
        .height = 16,
        .target_page = UI_PAGE_HOME,
        .on_select = NULL
    },  // 跳转，无自定义动作
    { 
        .text = "日期时间设置",
        .font_size = OLED_8X16,
        .x = 0,
        .y = 16,
        .width = 96,
        .height = 16,
        .target_page = UI_PAGE_SETTING_DATETIME,
        .on_select = NULL
    } // 不跳转
};

static void Page_Setting_Draw(UI_Page *self)
{
    // 绘制内容
    OLED_Clear();
    OLED_ShowImage(self->items[0].x,self->items[0].y,self->items[0].width,self->items[0].height,self->items[0].text); // Return
    OLED_ShowString(self->items[1].x,self->items[1].y,self->items[1].text,self->items[1].font_size);                  // 日期时间设置

     // 绘制焦点反显
    OLED_ReverseArea(self->items[self->focus_index].x,
                     self->items[self->focus_index].y,
                     self->items[self->focus_index].width,
                     self->items[self->focus_index].height);
	OLED_Update();
}
static void Page_Setting_key(UI_Page *self, KeyEventInfo_TypeDef *key)
{
    if (key->event != KEY_EVENT_SHORT) return;

    switch(key->type)
    {
        case KEY_UP:
            self->focus_index = (self->focus_index + self->item_count - 1) % self->item_count;
            Page_Setting_Draw(self);  // 重绘整个页面
            break;
            
        case KEY_DOWN:
            self->focus_index = (self->focus_index + 1) % self->item_count;
            Page_Setting_Draw(self);
            break;
            
        case KEY_ENTER: {
            UI_Page_TypeDef target = self->items[self->focus_index].target_page;
            if (target != UI_PAGE_MAX) {
                UI_SwitchPage(target);
            } else if (self->items[self->focus_index].on_select) {
                self->items[self->focus_index].on_select(self);
            }
            break;
        }
        default:
            break;
    }
}
static void Page_Setting_OnEnter(UI_Page *self) {
    Page_Setting_Draw(self);
}

void Page_Setting_Init(void) {
    page_setting.page_state = UI_PAGE_SETTING;
    page_setting.vtable = &setting_vtable;
    page_setting.focus_index = 0;
    page_setting.items = menu_items;
    page_setting.item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    page_setting.refresh_interval = 0;
    page_setting.last_refresh = 0;

    UI_RegisterPage(&page_setting);
}