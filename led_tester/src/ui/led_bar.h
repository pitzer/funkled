#ifndef LV_led_bar_H
#define LV_led_bar_H

#include "../lvgl.h"
#include "led_pattern.h"

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

// Create a new LED bar widget
lv_obj_t* led_bar_create(
    lv_obj_t* parent,
    uint32_t num_leds,
    uint32_t channel,
    const led_pattern_func_t pattern_update,
    const CRGBPalette16* palette,
    const uint32_t* period_ms,
    const uint8_t* brightness
);

#endif /*LV_led_bar_H*/