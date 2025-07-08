
#include "cached_pattern.h"

static const uint32_t test_num_leds_per_segment[] = {1};
static const PATTERN_ATTRIBUTE_LARGE_CONST CRGB test_pixels[] = {
    {0xFF, 0x00, 0x00}, {0xFF, 0x00, 0x00}};

constexpr cached_pattern_t test = {
    .color_ordering = WS2811_RGB,
    .animation_steps = 2,
    .num_segments = 1,
    .num_leds_per_segment = test_num_leds_per_segment,
    .num_pixels = sizeof(test_pixels) / sizeof(CRGB),
    .pixels = test_pixels,
};

CHECK_CACHED_PATTERN(test);
