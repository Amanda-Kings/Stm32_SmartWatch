#include "ui_page_flashlight.h"
#include "bsp_led.h"


static void Page_Flashlight_Draw(UI_Page *self);
static void Page_Flashlight_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_Flashlight_OnEnter(UI_Page *self);

// 主页虚函数表
static const UI_Page_VTable flashlight_vtable = {
    .on_enter = Page_Flashlight_OnEnter,
    .draw = Page_Flashlight_Draw,
    .on_key_event = Page_Flashlight_key
};
// 菜单项定义
static const UI_MenuItem menu_items[] = {
    { .text = Return, .x = 0, .y = 0, .width = 16, .height = 16 , .target_page = UI_PAGE_MENU}, // 返回图标
    { .text = "OFF", .font_size = OLED_12X24, .x = 20, .y = 20, .width = 36, .height = 24 , .target_page = UI_PAGE_MAX},
    { .text = "ON", .font_size = OLED_12X24, .x = 72, .y = 20, .width = 24, .height = 24 , .target_page = UI_PAGE_MAX},
};

static void Page_Flashlight_Draw(UI_Page *self)
{
    // 绘制内容
    OLED_Clear();
    OLED_ShowImage(self->items[0].x, self->items[0].y, 16, 16, Return); // 返回图标
    // 绘制菜单项
    for(uint8_t i=1; i<self->item_count; i++)
    {
        OLED_ShowString(self->items[i].x,self->items[i].y,self->items[i].text,self->items[i].font_size); // 菜单项
    }
     // 绘制焦点反显
    OLED_ReverseArea(self->items[self->focus_index].x,
                     self->items[self->focus_index].y,
                     self->items[self->focus_index].width,
                     self->items[self->focus_index].height);
    OLED_Update();
}
static void Page_Flashlight_OnEnter(UI_Page *self)
{
    Page_Flashlight_Draw(self);
}
static void Page_Flashlight_key(UI_Page *self, KeyEventInfo_TypeDef *key)
{
    if (key->event != KEY_EVENT_SHORT) return;
    switch (key->type)
    {
        case KEY_UP:
            self->focus_index = (self->focus_index + self->item_count - 1) % self->item_count;
            Page_Flashlight_Draw(self);
            break;
        case KEY_DOWN:
            self->focus_index = (self->focus_index + 1) % self->item_count;
            Page_Flashlight_Draw(self);
            break;
        case KEY_ENTER:
            if(self->focus_index == 0) // 返回
            {
                UI_SwitchPage(UI_PAGE_MENU);
            }
            else
            {
                // 模拟控制手电筒开关（实际应用中这里会调用硬件接口）
                if(self->focus_index == 1) // OFF
                {
                    // 关闭手电筒
                    // 例如：Flashlight_SetState(false);
                    LED_Off(); // 这里用LED模拟手电筒关闭

                }
                else if(self->focus_index == 2) // ON
                {
                    // 打开手电筒
                    // 例如：Flashlight_SetState(true);
                    LED_On(); // 这里用LED模拟手电筒打开

                }
            }
    }
}

void Page_Flashlight_Init(void)
{
    static UI_Page page_flashlight;

    page_flashlight.page_state = UI_PAGE_FLASHLIGHT;
    page_flashlight.vtable = &flashlight_vtable;
    page_flashlight.focus_index = 0;
    page_flashlight.items = menu_items; // 菜单页
    page_flashlight.item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    page_flashlight.data = NULL;
    page_flashlight.refresh_interval = 0;
    page_flashlight.last_refresh = 0;
    UI_RegisterPage(&page_flashlight);
}