#ifndef LV_led_bar_H
#define LV_led_bar_H

#include "../lvgl.h"
#include "led_pattern.h"

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

/**********************
 * GLOBAL PROTOTYPES
 **********************/
lv_obj_t* led_bar_create(lv_obj_t* parent, uint32_t num_leds, led_pattern_t* pattern, const CRGBPalette16* palette, uint32_t period_ms);

#endif /*LV_led_bar_H*/