#ifndef LED_ARRAY_H
#define LED_ARRAY_H

#include "led_pattern.h"
#include "led_palette.h"
#include <OctoWS2811.h>
#include <stdint.h>

//
// Constants
//
// The number of LED channels that we are managing
const uint32_t num_led_channels = 16;
// The maximum number of LEDs per LED string
const uint32_t max_leds_per_channel = 1024;
// The total number of LEDs
const uint32_t max_leds = num_led_channels * max_leds_per_channel;
// Special value to use the current channel
const uint32_t CHANNEL_CURRENT = 0xFFFFFFFF;

//
// Typedefs
//
// Descriptor for an LED string
typedef struct {
    // Number of LEDs in the strip
    uint32_t num_leds;
    // RGB channels order. Use the OctoWS2811 constants
    uint8_t color_ordering;
    // The index of the pattern currently used by the strip
    uint32_t pattern_index;
    // The index of the palette currently used by the strip
    uint32_t palette_index;
    // The period of the pattern update in milliseconds
    uint32_t update_period_ms;
    // The brightness of the strip
    uint8_t brightness;
} led_string_t;

//
// Globals
//
// The LED string descriptors
extern led_string_t led_strings[num_led_channels];
// The currently selected LED channel
extern uint32_t current_channel;

//
// Functions
//
void led_array_init();

// Get the current pattern
inline led_pattern_t* current_pattern() {
    return &led_patterns[led_strings[current_channel].pattern_index];
}

// Get the current palette
inline CRGBPalette16* current_palette() {
    return &led_palettes[led_strings[current_channel].palette_index].palette;
}

#endif // LED_PALETTE_H