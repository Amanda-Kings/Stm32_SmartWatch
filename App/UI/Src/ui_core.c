#include "ui_core.h"

static UI_Page *g_current_page = NULL;   // 当前活动页面

// 页面切换
void UI_SwitchPage(UI_Page *new_page) {
    g_current_page = new_page;
    if (g_current_page && g_current_page->vtable->draw) {
        g_current_page->vtable->draw(g_current_page);
    }
}



// 按键事件处理
void UI_OnKeyEvent(KeyObject_TypeDef *key) {
    if (g_current_page && g_current_page->vtable->on_key_event) {
        g_current_page->vtable->on_key_event(g_current_page, key);
    }
}