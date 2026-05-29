#ifndef SLIDE_MENU_H
#define SLIDE_MENU_H

#include <stdint.h>
#include <stdbool.h>

/* 动画参数 */
#define ANIM_STEP_SIZE  10       // 每帧移动像素（48/8=6，8帧完成）
#define ITEM_WIDTH      48      // 菜单项宽度（图标32+间距16）
#define ICON_WIDTH      32      // 图标实际宽度
#define ICON_HEIGHT     32      // 图标高度

typedef struct {
    int16_t position;           // 当前内容偏移（像素）
    int16_t target_position;    // 动画目标偏移
    int8_t  selected_index;     // 当前选中索引（仅在非动画时有效）
    int8_t  target_index;       // 动画目标索引
    uint8_t is_animating;       // 动画进行中标志
    uint8_t item_count;         // 菜单项总数
    uint8_t screen_width;       // 屏幕宽度
} SlideMenu;

/* 初始化 */
void SlideMenu_Init(SlideMenu *menu, uint8_t item_count, uint8_t screen_width);

/* 每帧调用更新动画 */
void SlideMenu_Update(SlideMenu *menu);

/* 触发移动：方向 0=左（索引+1），1=右（索引-1） */
bool SlideMenu_Move(SlideMenu *menu, uint8_t direction);

/* 获取当前选中索引（仅非动画时有效，动画期间返回 -1） */
int8_t SlideMenu_GetSelectedIndex(const SlideMenu *menu);

/* 绘制菜单 */
void SlideMenu_Draw(const SlideMenu *menu,
                    void (*draw_icon)(uint8_t index, int16_t x, int16_t y));

#endif