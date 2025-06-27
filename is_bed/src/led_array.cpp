#include "led_array.h"
#include <EEPROM.h>

uint32_t current_channel = 0;

static const uint16_t magic = 0xCAFE;

static const uint8_t default_color_ordering = WS2811_GRB;
static const uint32_t default_pattern_index = 0;
static const CRGB default_single_color = CRGB::Red;
static const int32_t default_palette_index = 0;
static const int32_t default_update_period_ms = 3000;
static const uint8_t default_brightness = 255;

#define ZONE_CAGE 0
#define ZONE_CENTER 1
#define ZONE_FRONT 2
#define ZONE_HEADBOARD 3

CRGB leds_crgb[max_leds_per_channel];

led_zone_t led_zones[] = {

    {
        .name = "Center",
        .pattern_index = default_pattern_index,
        .single_color = default_single_color,
        .palette_index = default_palette_index,
        .update_period_ms = default_update_period_ms,
        .brightness = default_brightness,
    },
    {
        .name = "Front",
        .pattern_index = default_pattern_index,
        .single_color = default_single_color,
        .palette_index = default_palette_index,
        .update_period_ms = default_update_period_ms,
        .brightness = default_brightness,
    },
    {
        .name = "Headboard",
        .pattern_index = default_pattern_index,
        .single_color = default_single_color,
        .palette_index = default_palette_index,
        .update_period_ms = default_update_period_ms,
        .brightness = default_brightness,
    },
    {
        .name = "Cage",
        .pattern_index = default_pattern_index,
        .single_color = default_single_color,
        .palette_index = default_palette_index,
        .update_period_ms = default_update_period_ms,
        .brightness = default_brightness,
    },
};

led_segment_t post_front_left_segments[] = {
    {
        .name = "Cage Left",
        .num_leds = 12,
        .string_offset = 0,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Cage Right",
        .num_leds = 12,
        .string_offset = 12,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Frame Right",
        .num_leds = 21,
        .string_offset = 24,
    },
    {
        .name = "Frame Left",
        .num_leds = 21,
        .string_offset = 45,
    },
};

led_segment_t post_front_right_segments[] = {
    {
        .name = "Cage Left",
        .num_leds = 12,
        .string_offset = 0,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Cage Right",
        .num_leds = 12,
        .string_offset = 12,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Frame Right",
        .num_leds = 21,
        .string_offset = 24,
        .zone = ZONE_CENTER,
    },
    {
        .name = "Frame Left",
        .num_leds = 21,
        .string_offset = 45,
        .zone = ZONE_CENTER,
    },
};

led_segment_t post_rear_left_segments[] = {
    {
        .name = "Cage Left",
        .num_leds = 12,
        .string_offset = 0,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Cage Right",
        .num_leds = 12,
        .string_offset = 12,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Back Right",
        .num_leds = 21,
        .string_offset = 24,
        .zone = ZONE_FRONT,
    },
    {
        .name = "Back Left",
        .num_leds = 21,
        .string_offset = 45,
        .zone = ZONE_FRONT,
    },
    {
        .name = "Frame Right",
        .num_leds = 21,
        .string_offset = 66,
        .zone = ZONE_CENTER,
    },
    {
        .name = "Frame Left",
        .num_leds = 21,
        .string_offset = 87,
        .zone = ZONE_CENTER,
    },
};

led_segment_t post_rear_right_segments[] = {
    {
        .name = "Cage Left",
        .num_leds = 12,
        .string_offset = 0,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Cage Right",
        .num_leds = 12,
        .string_offset = 12,
        .zone = ZONE_CAGE,
    },
    {
        .name = "Back Right",
        .num_leds = 21,
        .string_offset = 24,
        .zone = ZONE_FRONT,
    },
    {
        .name = "Back Left",
        .num_leds = 21,
        .string_offset = 45,
        .zone = ZONE_FRONT,
    },
    {
        .name = "Frame Right",
        .num_leds = 21,
        .string_offset = 66,
        .zone = ZONE_CENTER,
    },
    {
        .name = "Frame Left",
        .num_leds = 21,
        .string_offset = 87,
        .zone = ZONE_CENTER,
    },
};

led_string_t led_strings[] = {
    {
        .name = "Post Front Left",
        .num_segments = sizeof(post_front_left_segments) / sizeof(post_front_left_segments[0]),
        .segments = post_front_left_segments,
    },
    {
        .name = "Post Front Right",
        .num_segments = sizeof(post_front_right_segments) / sizeof(post_front_right_segments[0]),
        .segments = post_front_right_segments,
    },
    {
        .name = "Post Rear Left",
        .num_segments = sizeof(post_rear_left_segments) / sizeof(post_rear_left_segments[0]),
        .segments = post_rear_left_segments,
    },
    {
        .name = "Post Rear Right",
        .num_segments = sizeof(post_rear_right_segments) / sizeof(post_rear_right_segments[0]),
        .segments = post_rear_right_segments,
    },
};

void led_array_init()
{
}

void led_array_save() {
    // Write the magic
    EEPROM.write(0, magic & 0xFF);
    EEPROM.write(1, magic >> 8);
    // Write the size
    uint16_t size = sizeof(led_strings);
    EEPROM.write(2, size & 0xFF);
    EEPROM.write(3, size >> 8);
    // Write the data
    EEPROM.put(4, led_strings);
}

void led_array_load() {
    // Read the magic
    uint16_t magic_read = EEPROM.read(0) | (EEPROM.read(1) << 8);
    if (magic_read != magic) {
        return;
    }
    // Read the size
    uint16_t size = EEPROM.read(2) | (EEPROM.read(3) << 8);
    if (size != sizeof(led_strings)) {
        return;
    }
    // Read the data
    EEPROM.get(4, led_strings);
}