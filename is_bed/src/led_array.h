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
const uint32_t num_led_channels = 5;
// The number of LED zones
const uint32_t num_zones = 4;
// The maximum number of LEDs per LED string
const uint32_t max_leds_per_channel = 1024;
// The total number of LEDs
const uint32_t max_leds = num_led_channels * max_leds_per_channel;
// Special value to use the current channel
const uint32_t CHANNEL_CURRENT = 0xFFFFFFFF;

//
// Typedefs
//
// Descriptor for a segment of an LED string. Multiple segments for a string.
typedef struct
{
    // The name of the segment.
    const char *name;
    // Number of LEDs in the segment
    uint32_t num_leds;
    // Offset in number of LEDs where the segment starts within the LED string
    uint32_t string_offset;
    // The zone this segment belongs to.
    uint8_t zone;
} led_segment_t;

// Descriptor for an LED string.
typedef struct
{
    // The name of the string.
    const char *name;
    // Number of segments in the strip
    uint32_t num_segments;
    // The segment descriptors
    led_segment_t *segments;
} led_string_t;

// Descriptor of a zone. A zone consists of multiple LED segments, 
// potentially accross multiple LED strings. Each zone has a unified
// pattern and brightness.
typedef struct
{
    // The name of the zone.
    const char *name;
    // The index of the pattern currently used by the segment
    uint32_t pattern_index;
    // The color for non-palette patterns
    CRGB single_color;
    // The color ordering of the strip. Uses the OctoWS2811 constants
    uint8_t color_ordering;
    // The index of the palette currently used by the segment
    uint32_t palette_index;
    // The period of the pattern update in milliseconds
    uint32_t update_period_ms;
    // The brightness of the segment
    uint8_t brightness;
} led_zone_t;

//
// Globals
//
// The LED string descriptors
extern led_zone_t led_zones[];
// The LED string descriptors
extern led_string_t led_strings[];
// The currently selected LED channel
extern uint32_t current_channel;
// Temporary buffer to render LEDs of a channel.
extern CRGB leds_crgb[max_leds_per_channel];

//
// Functions
//
void led_array_init();
void led_array_save();
void led_array_load();

#endif // LED_PALETTE_H