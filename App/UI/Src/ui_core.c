#include "ui_core.h"
#include "ui_page_home.h"
#include "bsp_sys_tick.h"
#include <stddef.h>

static UI_Page *page_table[UI_PAGE_MAX] = {NULL};
static UI_Page *current_page = NULL;   // 当前活动页面

// 页面切换
void UI_SwitchPage(UI_Page_TypeDef target) {
    if (target < UI_PAGE_MAX && page_table[target] != NULL) {
        current_page = page_table[target];
        current_page->last_refresh = SysTick_GetTick();
        if (current_page->vtable && current_page->vtable->draw) {
            current_page->vtable->draw(current_page);
        }
    }
}

// 按键事件处理
void UI_HandleKeyEvent(KeyEventInfo_TypeDef *info) {
    if (current_page && current_page->vtable->on_key_event) {
        current_page->vtable->on_key_event(current_page, info);
    }
}

void UI_Refresh(void) {
    if (current_page == NULL || current_page->refresh_interval == 0) {
        return;
    }
    
    uint32_t now = SysTick_GetTick();
    if (now - current_page->last_refresh >= current_page->refresh_interval) {
        current_page->last_refresh = now;
        if (current_page->vtable && current_page->vtable->draw) {
            current_page->vtable->draw(current_page);
        }
    }
}

void UI_Init(void) {
    for (int i = 0; i < UI_PAGE_MAX; i++) {
        page_table[i] = NULL;
    }
    current_page = NULL;
}

void UI_RegisterPage(UI_Page *page) {
    if (page && page->page_state < UI_PAGE_MAX) {
        page_table[page->page_state] = page;
    }
}

UI_Page UI_GetCurrentPage(void) {
    return *current_page;
}

UI_Page UI_GetPage(UI_Page_TypeDef page_id) {
    return *page_table[page_id];
}

//设置页面刷新时间（ms）
void SetPageRefreshInterval(UI_Page_TypeDef page_id,uint16_t interval)
{
    page_table[page_id]->refresh_interval = interval;
}
