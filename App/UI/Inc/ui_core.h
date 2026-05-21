#ifndef UI_CORE_H
#define UI_CORE_H
#include "bsp_key.h"
#include "bsp_oled.h"

/* 前向声明 */
typedef struct UI_Page UI_Page;

/* 页面ID */
typedef enum
{
    UI_PAGE_HOME = 0,
    UI_PAGE_MENU,
    UI_PAGE_SETTING,
    UI_PAGE_MAX
} UI_Page_TypeDef;

/* 菜单项定义 */
typedef struct
{
    const char *text;
    uint8_t font_size;                // 字体大小，参考bsp_oled.h中的FontSize定义
    uint8_t x;                        // x坐标
    uint8_t y;                        // y坐标
    uint8_t width;                    // 菜单项宽度（用于焦点显示）
    uint8_t height;                   // 菜单项高度（用于焦点显示）
    UI_Page_TypeDef target_page;      // 跳转目标（UI_PAGE_MAX 表示不跳转）
    void (*on_select)(UI_Page *self); // 选中后的回调，传递页面对象自己
} UI_MenuItem;

/* 页面虚函数表 */
typedef struct
{
    void (*draw)(UI_Page *self);                                 // 全屏绘制
    void (*on_key_event)(UI_Page *self, KeyEventInfo_TypeDef *key); // 自定义按键处理
}UI_Page_VTable;

/* 页面基类 */
struct UI_Page
{
    UI_Page_TypeDef page_state;    // 页面状态
    const UI_Page_VTable *vtable; // 虚函数表
    uint8_t focus_index;          // 当前焦点索引
    const UI_MenuItem *items;     // 菜单项数组（非菜单页为 NULL）
    uint8_t item_count;           // 菜单项个数
    void *data;                   // 子类私有数据
    uint16_t refresh_interval;  // 刷新间隔(ms)，0表示不需要
    uint32_t last_refresh;      // 上次刷新的tick
};

/* 核心调度接口 */
void UI_Init(void);
void UI_RegisterPage(UI_Page *page);
void UI_Refresh(void);
UI_Page UI_GetCurrentPage(void);
UI_Page UI_GetPage(UI_Page_TypeDef page_id);
void SetPageRefreshInterval(UI_Page_TypeDef page_id,uint16_t interval);
void UI_SwitchPage(UI_Page_TypeDef target);      // 切换到新页面
void UI_HandleKeyEvent(KeyEventInfo_TypeDef *info); // 按键事件入口
#endif