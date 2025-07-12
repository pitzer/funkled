/**
 * @file cached_pattern.h
 *
 */

#ifndef CACHED_PATTERN_H
#define CACHED_PATTERN_H

#include <OctoWS2811.h>
#include <FastLED.h>



// Struct to describe a cached pattern resource.
typedef struct
{
    // Two bytes indicating pattern format.
    uint16_t magic;
    // The color ordering used by the pattern
    uint8_t color_ordering;
    // The number of animation steps stored in the pattern.
    uint32_t animation_steps;
    // Number of segments in this cached pattern.
    uint32_t num_segments;
    // Number of LEDs in each pattern segment.
    const uint32_t *num_leds_per_segment;
    // Size of the pattern in bytes
    uint32_t num_pixels;
    // Number of bytes in one animation step 
    uint32_t stride;
} cached_pattern_header_t;

typedef struct
{
    cached_pattern_header_t header;
    // Size of the image in bytes
    uint32_t data_size;
    const void *path;
} cached_pattern_t;

// Attribute to mark large constant binary arrays used in cached patterns
#define PATTERN_ATTRIBUTE_LARGE_CONST __attribute__((section(".progmem")))

// Macro to validate data consistency
#define CHECK_CACHED_PATTERN(p)                                     \
    constexpr uint32_t num_leds =                                   \
        total_pattern_leds(p.num_segments, p.num_leds_per_segment); \
    constexpr uint32_t num_pixels = p.animation_steps * num_leds;   \
    static_assert(num_pixels == p.num_pixels);

// Static function to count total number of LEDs addressed by the pattern.
constexpr uint32_t total_pattern_leds(const uint32_t num_segments,
                                      const uint32_t *num_leds_per_segment,
                                      std::size_t i = 0U)
{
    return i < num_segments ? (num_leds_per_segment[i] +
                               total_pattern_leds(num_segments,
                                                  num_leds_per_segment,
                                                  i + 1U))
                            : 0;
}

extern const cached_pattern_t fire;
extern const cached_pattern_t rainbow;

#endif /*CACHED_PATTERN_H*/
