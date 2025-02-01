#ifndef LV_COLOR_SELECTOR_H
#define LV_COLOR_SELECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl.h"

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* color_selector_create(lv_obj_t* parent);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_COLOR_SELECTOR_H*/