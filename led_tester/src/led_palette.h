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

#endif // LED_PALETTE_H

