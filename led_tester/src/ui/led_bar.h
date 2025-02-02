#ifndef LV_led_bar_H
#define LV_led_bar_H

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
lv_obj_t* led_bar_create(lv_obj_t* parent, uint32_t num_leds);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_led_bar_H*/