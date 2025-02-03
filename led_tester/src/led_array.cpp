#include "led_array.h"

led_string_t led_strings[num_led_channels];

uint32_t current_channel = 0;

void led_array_init() {
    for (uint32_t i = 0; i < num_led_channels; i++) {
        led_strings[i].num_leds = 100;
        led_strings[i].color_ordering = WS2811_GRB;
        led_strings[i].pattern_index = 0;
        led_strings[i].palette_index = 0;
        led_strings[i].update_period_ms = 3000;
        led_strings[i].brightness = 255;
    }
}
