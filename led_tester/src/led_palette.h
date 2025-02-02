#ifndef LED_PALETTE_H
#define LED_PALETTE_H

#include <FastLED.h>

// A small struct to describe a palette
typedef struct {
    // The name of the palette
    const char* name;
    // The description of the palette
    const char* desc;
    // The palette itself
    CRGBPalette16 palette;
} led_palette_t;

// All the palettes available
extern led_palette_t led_palettes[];

// Number of palettes available
extern uint32_t num_led_palettes();

// The current palette index
extern uint32_t palette_index;

// Get the current palette
inline CRGBPalette16* current_palette() {
    return &led_palettes[palette_index].palette;
}

#endif // LED_PALETTE_H

