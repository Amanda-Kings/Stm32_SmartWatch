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
    void (*on_key_event)(UI_Page *self, KeyObject_TypeDef *key); // 自定义按键处理（可选）
}UI_Page_VTable;

/* 页面基类 */
struct UI_Page
{
    const UI_Page_VTable *vtable; // 虚函数表
    uint8_t focus_index;          // 当前焦点索引
    const UI_MenuItem *items;     // 菜单项数组（非菜单页为 NULL）
    uint8_t item_count;           // 菜单项个数
    void *data;                   // 子类私有数据
};

/* 核心调度接口 */
void UI_SwitchPage(UI_Page *new_page);      // 切换到新页面
void UI_OnKeyEvent(KeyObject_TypeDef *key); // 按键事件入口

#endif