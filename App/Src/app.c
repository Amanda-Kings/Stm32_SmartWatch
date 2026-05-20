#include "bsp_key.h"
#include "ui_core.h"

void App_Init(void) {
    Key_Init();
    UI_Init();
    
    Key_RegisterCallback(KEY_UP,    UI_HandleKeyEvent);
    Key_RegisterCallback(KEY_DOWN,  UI_HandleKeyEvent);
    Key_RegisterCallback(KEY_ENTER, UI_HandleKeyEvent);
}