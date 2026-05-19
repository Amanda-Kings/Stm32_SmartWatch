#ifndef APP_UI_H
#define APP_UI_H

#include "stm32f10x_conf.h"
#include "bsp_key.h"

//UI 状态枚举
typedef enum {
    UI_PAGE_HOME = 0,    // 主页（菜单、设置）
    UI_PAGE_MENU,        // 菜单页
    UI_PAGE_SETTING,     // 设置页
    UI_PAGE_NONE         // 无页面（执行自定义动作）
} UI_Page_TypeDef;

// 菜单项描述
typedef struct {
    const char *text;               // 显示的文字
    uint8_t     font_size;          // 显示的文字大小，参考FontSize
    uint8_t     target_page;        // 按下后跳转的目标页面（UI_PAGE_NONE表示不跳转，执行自定义动作）
    void        (*on_select)(void); // 自定义动作函数（target_page为NONE时有效）
} UI_MenuItem;

// 页面描述
typedef struct {
    UI_Page_TypeDef page_state;             // 页面状态
    const UI_MenuItem *items;      // 菜单项数组
    uint8_t          item_count;    // 菜单项个数
    void (*draw)(UI_Page *page); // 绘制函数
} UI_Page;

typedef struct {
    const UI_Page *current_page;    // 当前页面指针
    uint8_t       focus_index;      // 当前页面中被选中的菜单项索引（0 ~ item_count-1）
} UI_State;


void UI_Init(void);
void UI_Task(void);                  // UI 主任务
void UI_HandleKey(KeyEvent_TypeDef e); // 按键事件交给UI

#endif // APP_UI_H