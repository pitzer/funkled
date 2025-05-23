#ifndef LV_led_bar_H
#define LV_led_bar_H

#include "../lvgl.h"
#include "led_pattern.h"
#include "led_palette.h"

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

// This struct is used to describe a sequence that can be displayed on the LEDs
// If any of the pointers are NULL, then the values in the currently selected channel are used
typedef struct {
    led_pattern_func_t pattern_update;
    const led_palette_t* palette;
    const CRGB* single_color;
    const uint32_t* period_ms;
    const uint8_t* brightness;
} led_bar_dsc_t;

// Create a new LED bar widget
lv_obj_t* led_bar_create(
    lv_obj_t* parent,
    uint32_t num_leds,
    uint32_t channel,
    const led_bar_dsc_t* dsc
);

#endif /*LV_led_bar_H*/