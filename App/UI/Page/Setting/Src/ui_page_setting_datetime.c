#include "ui_page_setting_datetime.h"
#include "bsp_rtc.h"

static UI_Page Page_SetDateTime;

/* ========== 枚举与结构体 ========== */
typedef enum {
    DATETIME_MODE_NAV,
    DATETIME_MODE_EDIT
} DateTimeMode;

typedef struct {
    const char *label;
    uint8_t fsize;
    uint8_t label_width;
    uint8_t label_height;
    uint8_t x;
    uint8_t y;
    uint8_t digit_count;
    uint16_t (*get_value)(const DateTime *time);
    void (*adjust)(DateTime *time, int8_t delta);
} DateTimeField;

typedef struct {
    DateTime time;
    DateTimeMode mode;
} SetDateTimeData;

/* ========== 静态函数声明 ========== */
static void Page_SetDateTime_Draw(UI_Page *self);
static void Page_SetDateTime_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_SetDateTime_OnEnter(UI_Page *self);

static const UI_Page_VTable SetDateTime_vtable = {
    .on_enter = Page_SetDateTime_OnEnter,
    .draw = Page_SetDateTime_Draw,
    .on_key_event = Page_SetDateTime_key
};

/* ========== 日期工具 ========== */
static uint8_t IsLeapYear(uint16_t year) {
    return ((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0);
}
static uint8_t GetDaysInMonth(uint16_t year, uint8_t month) {
    static const uint8_t days_in_month[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && IsLeapYear(year)) return 29;
    return days_in_month[month - 1];
}
static void ClampDay(DateTime *time) {
    uint8_t max_day = GetDaysInMonth(time->year, time->month);
    if (time->day > max_day) time->day = max_day;
}

/* ========== 字段取值 / 调整 ========== */
static uint16_t GetYear(const DateTime *t)   { return t->year; }
static uint16_t GetMonth(const DateTime *t)  { return t->month; }
static uint16_t GetDay(const DateTime *t)    { return t->day; }
static uint16_t GetHour(const DateTime *t)   { return t->hour; }
static uint16_t GetMinute(const DateTime *t) { return t->minute; }
static uint16_t GetSecond(const DateTime *t) { return t->second; }

static void AdjustYear(DateTime *t, int8_t delta) {
    int val = (int)t->year + delta;
    if (val < 2000) val = 2099;
    if (val > 2099) val = 2000;
    t->year = (uint16_t)val;
    ClampDay(t);
}
static void AdjustMonth(DateTime *t, int8_t delta) {
    int val = (int)t->month + delta;
    while (val < 1)  val += 12;
    while (val > 12) val -= 12;
    t->month = (uint8_t)val;
    ClampDay(t);
}
static void AdjustDay(DateTime *t, int8_t delta) {
    int val = (int)t->day + delta;
    while (val < 1)  val += GetDaysInMonth(t->year, t->month);
    while (val > GetDaysInMonth(t->year, t->month)) val -= GetDaysInMonth(t->year, t->month);
    t->day = (uint8_t)val;
}
static void AdjustHour(DateTime *t, int8_t delta)   { t->hour   = (t->hour   + delta + 24) % 24; }
static void AdjustMinute(DateTime *t, int8_t delta) { t->minute = (t->minute + delta + 60) % 60; }
static void AdjustSecond(DateTime *t, int8_t delta) { t->second = (t->second + delta + 60) % 60; }

/* ========== 字段描述表（现在完全由它管理所有日期时间字段） ========== */
static const DateTimeField datetime_fields[] = {
    { "年:",OLED_8X16,24,16, 0, 16, 4, GetYear,   AdjustYear   },
    { "月:",OLED_8X16,24,16, 0, 32, 2, GetMonth,  AdjustMonth  },
    { "日:",OLED_8X16,24,16, 0, 48, 2, GetDay,    AdjustDay    },
    { "时:",OLED_8X16,24,16, 0,  0, 2, GetHour,   AdjustHour   },
    { "分:",OLED_8X16,24,16, 0, 16, 2, GetMinute, AdjustMinute },
    { "秒:",OLED_8X16,24,16, 0, 32, 2, GetSecond, AdjustSecond },
};
#define DATETIME_FIELD_COUNT  (sizeof(datetime_fields) / sizeof(datetime_fields[0]))

/* ========== 辅助函数 ========== */
// 根据 focus_item 返回当前屏幕应绘制的起始字段索引
static uint8_t GetFirstVisibleField(uint8_t focus_item) {
    if (focus_item >= 1 && focus_item <= 3) return 0;   // 焦点在字段0~2，显示第一屏
    if (focus_item >= 4 && focus_item <= 6) return 3;   // 焦点在字段3~5，显示第二屏
    return 0;   // 焦点在返回图标，默认显示第一屏
}


/* ========== 菜单项（只保留一个返回图标） ========== */
static const UI_MenuItem menu_items[] = {
    { .text = Return, .x = 0, .y = 0, .width = 16, .height = 16 }
    // 不再包含任何字段项，字段完全由 datetime_fields[] 管理
};

/* ========== 页面数据实例 ========== */
static SetDateTimeData DateTimeData = {
    .time = {
        .year = RTC_StartTime_YEAR, .month = RTC_StartTime_MONTH, .day = RTC_StartTime_DAY,
        .hour = RTC_StartTime_HOUR, .minute = RTC_StartTime_MINUTE, .second = RTC_StartTime_SECOND
    },
    .mode = DATETIME_MODE_NAV,
};

/* ========== 绘制函数 ========== */
static void Page_SetDateTime_Draw(UI_Page *self) {
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    DateTime *time = &data->time;
    uint8_t first = GetFirstVisibleField(self->focus_index);

    OLED_Clear();

    // 1. 绘制返回图标（始终显示）
    OLED_ShowImage(menu_items[0].x, menu_items[0].y, menu_items[0].width, menu_items[0].height, menu_items[0].text);

    // 2. 绘制当前可见的三个字段
    for (uint8_t i = first; i < first + 3 && i < DATETIME_FIELD_COUNT; i++) {
        const DateTimeField *f = &datetime_fields[i];
        uint16_t val = f->get_value(time);
        OLED_Printf(f->x, f->y, f->fsize, "%s%0*d", f->label, f->digit_count, val);
    }

    // 3. 焦点反显
    if (self->focus_index == 0) {
        // 焦点在返回图标
        if (data->mode == DATETIME_MODE_NAV) {
            OLED_ReverseArea(menu_items[0].x, menu_items[0].y, menu_items[0].width, menu_items[0].height);
        }
    } else {
        // 焦点在字段上（focus_item 1~6）
        uint8_t field_idx = self->focus_index - 1;
        const DateTimeField *f = &datetime_fields[field_idx];


        if (data->mode == DATETIME_MODE_NAV) {
            // 导航模式：反显整个字段区域（标签+值）
            OLED_ReverseArea(f->x, f->y, f->label_width-8, f->label_height);
        } else {
            // 编辑模式：只反显值部分
            OLED_ReverseArea(f->x + f->label_width, f->y, f->digit_count * f->fsize, f->label_height);
        }
    }

    OLED_Update();
}

/* ========== 按键处理 - 导航模式 ========== */
static void HandleNavKey(UI_Page *self, KeyEventInfo_TypeDef *key) {
    SetDateTimeData *data = (SetDateTimeData *)self->data;

    if (key->event == KEY_EVENT_SHORT) {
        switch (key->type) {
            case KEY_UP:
             printf("KEY_UP");
                // focus_item 0~6 循环
                if (self->focus_index == 0) self->focus_index = DATETIME_FIELD_COUNT;
                else self->focus_index--;
                Page_SetDateTime_Draw(self);
                break;
            case KEY_DOWN:
             printf("KEY_DOWN");
                // 6 再往下回到 0
                if (self->focus_index == DATETIME_FIELD_COUNT) self->focus_index = 0;
                else self->focus_index++;
                Page_SetDateTime_Draw(self);
                break;
            case KEY_ENTER:
                if (self->focus_index == 0) {
                    // 返回上一级页面
                    UI_SwitchPage(UI_PAGE_SETTING);
                } else {
                    // 进入编辑模式
                    data->mode = DATETIME_MODE_EDIT;
                    Page_SetDateTime_Draw(self);
                }
                break;
            default:
                break;
        }
    }
}

/* ========== 按键处理 - 编辑模式 ========== */
static void HandleEditKey(UI_Page *self, KeyEventInfo_TypeDef *key) {
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    // 编辑时 focus_item 一定在 1~6，不会在返回图标上
    uint8_t field_idx = self->focus_index - 1;

    if (key->event == KEY_EVENT_SHORT ||
        (key->event == KEY_EVENT_REPEAT && (key->type == KEY_UP || key->type == KEY_DOWN))) {
        switch (key->type) {
            case KEY_UP:
                datetime_fields[field_idx].adjust(&data->time, 1);
                break;
            case KEY_DOWN:
                datetime_fields[field_idx].adjust(&data->time, -1);
                break;
            case KEY_ENTER:
                data->mode = DATETIME_MODE_NAV;          // 退出编辑
                RTC_SetTimeStamp(&data->time);            // 保存到 RTC
                break;
            default:
                break;
        }
        Page_SetDateTime_Draw(self);
    }
}

/* ========== 页面总按键回调 ========== */
static void Page_SetDateTime_key(UI_Page *self, KeyEventInfo_TypeDef *key) {
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    if (data->mode == DATETIME_MODE_EDIT) {
        HandleEditKey(self, key);
    } else {
        HandleNavKey(self, key);
    }
}

/* ========== 页面进入 ========== */
static void Page_SetDateTime_OnEnter(UI_Page *self) {
    SetDateTimeData *data = (SetDateTimeData *)self->data;
    data->time = RTC_GetTimeStamp();
    ClampDay(&data->time);
    data->mode = DATETIME_MODE_NAV;
    self->focus_index = 0;            // 每次进入焦点回到返回图标
    Page_SetDateTime_Draw(self);
}

/* ========== 页面初始化 ========== */
void Page_SetDateTime_Init(void) {
    Page_SetDateTime.page_state = UI_PAGE_SETTING_DATETIME;
    Page_SetDateTime.vtable = &SetDateTime_vtable;
    Page_SetDateTime.focus_index = 0;            
    Page_SetDateTime.items = menu_items;
    Page_SetDateTime.item_count = 1;             // 只有一个返回项
    Page_SetDateTime.data = &DateTimeData;
    Page_SetDateTime.refresh_interval = 0;
    Page_SetDateTime.last_refresh = 0;

    UI_RegisterPage(&Page_SetDateTime);
}