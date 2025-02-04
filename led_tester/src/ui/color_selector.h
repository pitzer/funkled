#ifndef LV_COLOR_SELECTOR_H
#define LV_COLOR_SELECTOR_H

#include "../lvgl.h"

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

// Create a color selector widget
lv_obj_t* color_selector_create(lv_obj_t* parent);

#endif /*LV_COLOR_SELECTOR_H*/