#include "slide_menu.h"

void SlideMenu_Init(SlideMenu *menu, uint8_t item_count, uint8_t screen_width) {
    menu->item_count = item_count;
    menu->screen_width = screen_width;
    menu->selected_index = 0;
    menu->target_index = 0;
    menu->position = 0;
    menu->target_position = 0;
    menu->is_animating = 0;
}

void SlideMenu_Update(SlideMenu *menu) {
    if (!menu->is_animating) return;

    // 线性移动
    if (menu->position < menu->target_position) {
        menu->position += ANIM_STEP_SIZE;
        if (menu->position > menu->target_position) {
            menu->position = menu->target_position;
        }
    } else if (menu->position > menu->target_position) {
        menu->position -= ANIM_STEP_SIZE;
        if (menu->position < menu->target_position) {
            menu->position = menu->target_position;
        }
    }

    // 动画完成
    if (menu->position == menu->target_position) {
        menu->selected_index = menu->target_index;
        menu->is_animating = 0;
    }
}

bool SlideMenu_Move(SlideMenu *menu, uint8_t direction) {
    if (menu->is_animating) return false;   // 动画中，不允许打断

    int8_t new_index = menu->selected_index;
    if (direction == 0) {        // 左移（显示下一个）
        if (menu->selected_index >= menu->item_count - 1) return false;
        new_index = menu->selected_index + 1;
    } else {                     // 右移（显示上一个）
        if (menu->selected_index <= 0) return false;
        new_index = menu->selected_index - 1;
    }

    menu->target_index = new_index; 
    menu->target_position = new_index * ITEM_WIDTH;
    menu->is_animating = 1;
    return true;
}

int8_t SlideMenu_GetSelectedIndex(const SlideMenu *menu) {
    if (menu->is_animating) return -1;   // 动画中无效
    return menu->selected_index;
}

void SlideMenu_Draw(const SlideMenu *menu,
                    void (*draw_icon)(uint8_t index, int16_t x, int16_t y)) {
    // 屏幕中心X坐标（图标左边缘应对齐的位置）
    int16_t center_x = (menu->screen_width / 2) - (ICON_WIDTH / 2);

    // 实际偏移量
    int16_t offset = menu->position;

    // 计算当前视觉中心图标索引（四舍五入）
    int16_t center_idx = (offset + ITEM_WIDTH / 2) / ITEM_WIDTH;
    if (center_idx < 0) center_idx = 0;
    if (center_idx >= menu->item_count) center_idx = menu->item_count - 1;

    // 绘制左中右三个图标
    for (int i = center_idx - 1; i <= center_idx + 1; i++) {
        if (i < 0 || i >= menu->item_count) continue;

        // 计算图标左边缘的屏幕X坐标
        int16_t x = (i * ITEM_WIDTH - offset) + center_x;
        // 简单裁剪
        if (x + ICON_WIDTH <= 0 || x >= menu->screen_width) continue;
        draw_icon(i, x, 16);   // Y坐标固定16（根据你的布局调整）
    }
}