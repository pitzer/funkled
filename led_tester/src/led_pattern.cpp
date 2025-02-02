#include "led_pattern.h"
#include <Arduino.h>

// The static pattern will just set all the LEDs to the palette color, regardless of time
void static_pattern(uint32_t time_ms, uint32_t period_ms, const CRGBPalette16* palette, uint32_t num_leds, CRGB* leds)
{
    for (uint32_t i = 0; i < num_leds; i++) {
        uint8_t palette_index = i * 255 / num_leds;
        leds[i] = ColorFromPalette(*palette, palette_index);
    }
}

// The rotate pattern will just rotate all the LEDs on a palette
void rotate_pattern(uint32_t time_ms, uint32_t period_ms, const CRGBPalette16* palette, uint32_t num_leds, CRGB* leds)
{
    uint64_t offset = time_ms * 255 / period_ms;
    for (uint32_t i = 0; i < num_leds; i++) {
        uint8_t palette_index = i * 255 / num_leds + offset;
        leds[i] = ColorFromPalette(*palette, palette_index);
    }
}

led_pattern_t led_patterns[] = {
    {
        .name = "Static",
        .desc = "Assign the LEDs to a static palette",
        .update = static_pattern,
    },
    {
        .name = "Rotate",
        .desc = "Rotate through a palette",
        .update = rotate_pattern,
    },
};

uint32_t num_led_patterns() {
    return sizeof(led_patterns) / sizeof(led_pattern_t);
}

uint32_t pattern_index = 0;
