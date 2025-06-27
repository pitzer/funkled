
#include "pattern_dsc.h"

static const PATTERN_ATTRIBUTE_LARGE_CONST uint8_t test_map[] = {
        0xFF, 0x00, 0x00};

const pattern_dsc_t test = {
    .header.cf = WS2811_RGB,
    .header.num_leds = 1,
    .data_size = sizeof(test_map),
    .data = test_map,
};