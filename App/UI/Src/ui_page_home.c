#include "ui_page_home.h"
#include "bsp_rtc.h"

static UI_Page page_home;

static void Page_Home_Draw(UI_Page *self);
static void Page_Home_key(UI_Page *self, KeyObject_TypeDef *key);

// 主页虚函数表
static const UI_Page_VTable home_vtable = {
    .draw = Page_Home_Draw,
    .on_key_event = Page_Home_key
};
// 主页私有时间数据
static DateTime RTC_Time = {0};
// 菜单项定义
static const UI_MenuItem menu_items[] = {
    { "菜单", OLED_8X16, 0, 48, 32, 16, UI_PAGE_SETTING, NULL },  // 跳转，无自定义动作
    { "设置", OLED_8X16, 96, 48, 32, 16, UI_PAGE_MAX, NULL }, // 不跳转
};
void Page_Home_Init(void) {
    RTC_GetTimeStamp(&RTC_Time);
    page_home.vtable = &home_vtable;
    page_home.focus_index = 0;
    page_home.items = menu_items;
    page_home.item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    page_home.data = &RTC_Time;
    
}
static void Page_Home_Draw(UI_Page *self)
{
    DateTime *time = (DateTime *)self->data;
    // 绘制主页内容
    OLED_Clear();
	OLED_Printf(0,0,OLED_6X8,"%d-%d-%d",time->year,time->month,time->day);
	OLED_Printf(16,16,OLED_12X24,"%02d:%02d:%02d",time->hour,time->minute,time->second);
    for(uint8_t i = 0; i < self->item_count; i++)
    {
        OLED_ShowString(self->items[i].x,self->items[i].y,self->items[i].text,self->items[i].font_size);
    }
	OLED_Update();
}
static void Page_Home_key(UI_Page *self, KeyObject_TypeDef *key)
{
    if (key->event != KEY_SHORT_PRESS) return;
    switch(key->type)
    {
        case KEY_UP:
            self->focus_index = (self->focus_index + self->item_count - 1) % self->item_count; // 上移
            //更新界面焦点显示（例如反转选中项）
            OLED_Clear();
            OLED_ReverseArea(self->items[self->focus_index].x,self->items[self->focus_index].y,
                self->items[self->focus_index].width,self->items[self->focus_index].height);
            OLED_Update();
            break;
        case KEY_DOWN:
            self->focus_index = (self->focus_index + 1) % self->item_count; // 下移
            //更新界面焦点显示（例如反转选中项）
            OLED_Clear();
            OLED_ReverseArea(self->items[self->focus_index].x,self->items[self->focus_index].y,
                self->items[self->focus_index].width,self->items[self->focus_index].height);
            OLED_Update();
            break;
        case KEY_ENTER:
            if(self->items[self->focus_index].target_page != UI_PAGE_MAX)
            {
                UI_SwitchPage((UI_Page *)self->items[self->focus_index].target_page); // 切换页面
            }
            else if(self->items[self->focus_index].on_select)
            {
                self->items[self->focus_index].on_select(self); // 执行自定义动作
            }
            break;
        default:
            break;
    }
}