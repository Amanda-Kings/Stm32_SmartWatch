#include "ui_page_stopwatch.h"
#include "stdbool.h"
#include "bsp_sys_tick.h"

static UI_Page page_stopwatch;


typedef struct
{
    uint32_t elapsed_ms;   // 累计毫秒，最大约49.7天
    bool     is_running;
    uint32_t last_tick;    // 上一次 SysTick 值
} StopWatchData_TypeDef;

static void ConvertMsToTime(uint32_t ms, uint8_t *h, uint8_t *m, uint8_t *s, uint16_t *remain_ms);
static void UpdateStopWatch(StopWatchData_TypeDef *data);
static void Page_StopWatch_Draw(UI_Page *self);
static void Page_StopWatch_key(UI_Page *self, KeyEventInfo_TypeDef *key);
static void Page_StopWatch_OnEnter(UI_Page *self);

// 主页虚函数表
static const UI_Page_VTable stopwatch_vtable = {
    .on_enter = Page_StopWatch_OnEnter,
    .draw = Page_StopWatch_Draw,
    .on_key_event = Page_StopWatch_key
};
// 菜单项定义
static const UI_MenuItem menu_items[] = {
    { .text = Return, .x = 0, .y = 0, .width = 16, .height = 16 , .target_page = UI_PAGE_HOME}, // 返回图标
    { .text = "开始", .font_size = OLED_8X16, .x = 8, .y = 44, .width = 32, .height = 16 , .target_page = UI_PAGE_MAX},
    { .text = "停止", .font_size = OLED_8X16, .x = 48, .y = 44, .width = 32, .height = 16 , .target_page = UI_PAGE_MAX},
    { .text = "清除", .font_size = OLED_8X16, .x = 88, .y = 44, .width = 32, .height = 16 , .target_page = UI_PAGE_MAX}
};



static void Page_StopWatch_Draw(UI_Page *self)
{
    StopWatchData_TypeDef *data = (StopWatchData_TypeDef *)self->data;
 
    UpdateStopWatch(data);
    uint8_t h, m, s;
    uint16_t ms;
    ConvertMsToTime(data->elapsed_ms, &h, &m, &s, &ms);

    // 绘制内容
    OLED_Clear();
    // 显示格式：HH:MM:SS.CC  (厘秒 = ms/10)
    OLED_Printf(16, 16, OLED_8X16, "%02d:%02d:%02d.%02d", h, m, s, ms/10);
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

static void ConvertMsToTime(uint32_t ms, uint8_t *h, uint8_t *m, uint8_t *s, uint16_t *remain_ms)
{
    *h = ms / 3600000;
    ms %= 3600000;
    *m = ms / 60000;
    ms %= 60000;
    *s = ms / 1000;
    *remain_ms = ms % 1000;
}
static void UpdateStopWatch(StopWatchData_TypeDef *data)
{
    if (!data->is_running)
        return;

    uint32_t now = SysTick_GetTick();
    uint32_t diff = now - data->last_tick;   // 自然处理滴答回绕(无符号减法)
    data->elapsed_ms += diff;
    data->last_tick = now;

    // 可选：限制最大 99:59:59.99 后停止
    if (data->elapsed_ms >= 35999999)   // 99h59m59s990ms
    {
        data->elapsed_ms = 35999999;
        data->is_running = false;
    }
}
/* ---------- 按键处理 ---------- */
static void KeyHandler(UI_Page *self, uint8_t index)
{
    StopWatchData_TypeDef *data = (StopWatchData_TypeDef *)self->data;

    switch (index)
    {
        case 1: // 开始
            data->is_running = true;
            data->last_tick = SysTick_GetTick();   // 重新同步基准
            self->refresh_interval = 30;           // 30ms刷新，支持厘秒显示
            self->focus_index = 2;                 // 切换焦点到“停止”上
            self->vtable->draw(self);              // 立即绘制第一帧
            break;
        case 2: // 停止
            data->is_running = false;
            self->refresh_interval = 0;
            self->focus_index = 1;                 // 切换焦点回“开始”上
            self->vtable->draw(self);              // 立即绘制第一帧
            break;
        case 3: // 清除
            data->is_running = false;
            data->elapsed_ms = 0;
            self->refresh_interval = 0;
            self->vtable->draw(self);           // 立即绘制第一帧   
            break;
    default:
        break;
    }
}

static void Page_StopWatch_key(UI_Page *self, KeyEventInfo_TypeDef *key)
{
    if (key->event != KEY_EVENT_SHORT) return;
    switch (key->type)
    {
        case KEY_UP:
            self->focus_index = (self->focus_index + self->item_count - 1) % self->item_count;
            Page_StopWatch_Draw(self);
            break;
        case KEY_DOWN:
            self->focus_index = (self->focus_index + 1) % self->item_count;
            Page_StopWatch_Draw(self);
            break;
        case KEY_ENTER:
            if(self->focus_index == 0) // 返回
            {
                /* 返回前保证停止自动刷新 */
                ((StopWatchData_TypeDef *)self->data)->is_running = false;
                self->refresh_interval = 0;
                UI_SwitchPage(UI_PAGE_MENU);
            }
            else
            {
                KeyHandler(self, self->focus_index);
            }
            break;
        default:
            break;
    }
}
static void Page_StopWatch_OnEnter(UI_Page *self)
{
    Page_StopWatch_Draw(self);
}
void Page_StopWatch_Init(void)
{
     /* 私有数据初始化 (静态存储期，初始值自动为 0) */
    static StopWatchData_TypeDef stopwatch_data;

    page_stopwatch.page_state = UI_PAGE_STOPWATCH;
    page_stopwatch.vtable = &stopwatch_vtable;
    page_stopwatch.focus_index = 0;
    page_stopwatch.items = menu_items; // 菜单页
    page_stopwatch.item_count = sizeof(menu_items) / sizeof(menu_items[0]);
    page_stopwatch.data = &stopwatch_data; // 可用于存储子类私有数据
    page_stopwatch.refresh_interval = 0;

    UI_RegisterPage(&page_stopwatch);
}