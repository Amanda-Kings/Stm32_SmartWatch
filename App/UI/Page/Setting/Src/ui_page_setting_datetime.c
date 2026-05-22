#include "ui_page_setting_datetime.h"
#include "bsp_rtc.h"


static UI_Page Page_SetDateTime;

static void Page_SetDateTime_Draw(UI_Page *self);
static void Page_SetDateTime_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_SetDateTime_OnEnter(UI_Page *self);

// 主页虚函数表
static const UI_Page_VTable SetDateTime_vtable = {
    .on_enter = Page_SetDateTime_OnEnter,
    .draw = Page_SetDateTime_Draw,
    .on_key_event = Page_SetDateTime_key
};

typedef enum {
    Edit_Year,
    Edit_Month,
    Edit_Day,
    Edit_Hour,
    Edit_Minute,
    Edit_Second,
    EditField_MAX
}EditField_TypeDef;
// 页面数据结构
typedef struct {
    DateTime time;
    EditField_TypeDef edit_field;  // 正在编辑的字段索引
    uint8_t is_editing;  // 是否正在编辑
} SetDateTimeData;

static SetDateTimeData DateTimeData = {
    .time = 
    {
        .year = RTC_StartTime_YEAR, .month = RTC_StartTime_MONTH, .day = RTC_StartTime_DAY,
        .hour = RTC_StartTime_HOUR, .minute = RTC_StartTime_MINUTE, .second = RTC_StartTime_SECOND
    },
    .edit_field = EditField_MAX,
    .is_editing = 0
};

static void EnterEditMode(UI_Page *self)
{
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    data->is_editing = 1;
    data->edit_field = self->focus_index - 1;  // 减去返回按钮的偏移
    self->vtable->draw(self);

}
// 菜单项定义
static const UI_MenuItem menu_items[] = 
{
     { .text = Return, .x = 0, .y = 0, .width = 16, .height = 16,
      .target_page = UI_PAGE_SETTING, .on_select = NULL },
    { .text = "年:", .font_size = OLED_8X16, .x = 0, .y = 16,
      .width = 16, .height = 16, .target_page = UI_PAGE_MAX, .on_select = EnterEditMode },
    { .text = "月:", .font_size = OLED_8X16, .x = 0, .y = 32,
      .width = 16, .height = 16, .target_page = UI_PAGE_MAX, .on_select = EnterEditMode },
    { .text = "日:", .font_size = OLED_8X16, .x = 0, .y = 48,
      .width = 16, .height = 16, .target_page = UI_PAGE_MAX, .on_select = EnterEditMode },
    { .text = "时:", .font_size = OLED_8X16, .x = 0, .y = 0,
      .width = 16, .height = 16, .target_page = UI_PAGE_MAX, .on_select = EnterEditMode },
    { .text = "分:", .font_size = OLED_8X16, .x = 0, .y = 16,
      .width = 16, .height = 16, .target_page = UI_PAGE_MAX, .on_select = EnterEditMode },
    { .text = "秒:", .font_size = OLED_8X16, .x = 0, .y = 32,
      .width = 16, .height = 16, .target_page = UI_PAGE_MAX, .on_select = EnterEditMode },
};
// 通用字段调整函数
static void AdjustField(DateTime *time, EditField_TypeDef field_type, int8_t delta) {
    switch (field_type) {
        case Edit_Year:  // year
            time->year += delta;
            if (time->year < 2000) time->year = 2099;
            if (time->year > 2099) time->year = 2000;
            break;
        case Edit_Month:  // month
            time->month = (time->month + delta + 11) % 12 + 1;
            break;
        case Edit_Day:  // day
            time->day = (time->day + delta + 30) % 31 + 1;
            break;
        case Edit_Hour:  // hour
            time->hour = (time->hour + delta + 24) % 24;
            break;
        case Edit_Minute:  // minute
            time->minute = (time->minute + delta + 60) % 60;
            break;
        case Edit_Second:  // second
            time->second = (time->second + delta + 60) % 60;
            break;
    }
}
static void Page_SetDateTime_Draw(UI_Page *self)
{
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    DateTime *time = &data->time;
    // 绘制内容
    OLED_Clear();
    

    if(self->focus_index<4)
    {
        OLED_ShowImage(self->items[0].x,self->items[0].y,self->items[0].width,self->items[0].height,self->items[0].text); // Return
        OLED_Printf(self->items[1].x,self->items[1].y,self->items[1].font_size,"%s%04d",self->items[1].text,time->year);
        OLED_Printf(self->items[2].x,self->items[2].y,self->items[2].font_size,"%s%02d",self->items[2].text,time->month);
        OLED_Printf(self->items[3].x,self->items[3].y,self->items[3].font_size,"%s%02d",self->items[3].text,time->day);
    }
    else
    {
        OLED_Printf(self->items[4].x,self->items[4].y,self->items[4].font_size,"%s%02d",self->items[4].text,time->hour);
        OLED_Printf(self->items[5].x,self->items[5].y,self->items[5].font_size,"%s%02d",self->items[5].text,time->minute);
        OLED_Printf(self->items[6].x,self->items[6].y,self->items[6].font_size,"%s%02d",self->items[6].text,time->second);
    }
     // 绘制焦点反显
    if(data->is_editing)
    {
        if(data->edit_field == Edit_Year){
            OLED_ReverseArea(self->items[self->focus_index].x+OLED_8X16*3,
                            self->items[self->focus_index].y,
                            self->items[self->focus_index].width*2,
                            self->items[self->focus_index].height);
        }
        else
        {
            OLED_ReverseArea(self->items[self->focus_index].x+OLED_8X16*3,
                            self->items[self->focus_index].y,
                            self->items[self->focus_index].width,
                            self->items[self->focus_index].height);
        }
    }
    else
    {
    
    OLED_ReverseArea(self->items[self->focus_index].x,
                     self->items[self->focus_index].y,
                     self->items[self->focus_index].width,
                     self->items[self->focus_index].height);
    }
	OLED_Update();
}



static void Page_SetDateTime_key(UI_Page *self, KeyEventInfo_TypeDef *key)
{
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    if (key->event == KEY_EVENT_SHORT || key->event == KEY_EVENT_REPEAT)
    {
        if(data->is_editing)
        {
            if(key->event == KEY_EVENT_REPEAT && (key->type != KEY_UP && key->type != KEY_DOWN)) return;
            // 编辑模式：上下键调整值
            switch (key->type) {
                case KEY_UP:
                    AdjustField(&data->time, data->edit_field, 1);
                    break;
                case KEY_DOWN:
                    AdjustField(&data->time, data->edit_field, -1);
                    break;
                case KEY_ENTER:
                    data->is_editing = 0;  // 保存并退出编辑
                    RTC_SetTimeStamp(&data->time);
                    break;
            }
            self->vtable->draw(self);
        }
        else
        {
            if(key->event != KEY_EVENT_SHORT ) return;
            // 普通菜单导航模式
            switch(key->type)
            {
                case KEY_UP:
                    self->focus_index = (self->focus_index + self->item_count - 1) % self->item_count;
                    Page_SetDateTime_Draw(self);  // 重绘整个页面
                    break;
                    
                case KEY_DOWN:
                    self->focus_index = (self->focus_index + 1) % self->item_count;
                    Page_SetDateTime_Draw(self);
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
    }
    
}
// 切换到该页面时调用
static void Page_SetDateTime_OnEnter(UI_Page *self) {
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    data->time = RTC_GetTimeStamp();
    Page_SetDateTime_Draw(self);
}


void Page_SetDateTime_Init(void) {
    Page_SetDateTime.page_state = UI_PAGE_SETTING_DATETIME;
    Page_SetDateTime.vtable = &SetDateTime_vtable;
    Page_SetDateTime.focus_index = 0;
    Page_SetDateTime.items = menu_items;
    Page_SetDateTime.item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    Page_SetDateTime.data = &DateTimeData;
    Page_SetDateTime.refresh_interval = 0;
    Page_SetDateTime.last_refresh = 0;

    UI_RegisterPage(&Page_SetDateTime);
}