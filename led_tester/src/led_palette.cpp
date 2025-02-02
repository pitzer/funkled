#include "led_palette.h"
#include <Arduino.h>

led_palette_t led_palettes[] = {
    {
        .name = "Solid",
        .desc = "A solid color.",
        .palette = CRGBPalette16(CRGB::White),
    },
    {
        .name = "Rainbow",
        .desc = "Colors of the rainbow.",
        .palette = RainbowColors_p,
    },
    {
        .name = "Rainbow Stripe",
        .desc = "Colors of the rainbow with a stripe moving across.",
        .palette = RainbowStripeColors_p,
    },
    {
        .name = "Forest",
        .desc = "A walk in the forest...",
        .palette = ForestColors_p,
    },
    {
        .name = "Ocean",
        .desc = "A cruise on the ocean...",
        .palette = OceanColors_p,
    },
    {
        .name = "Lava",
        .desc = "A trip to the volcano...",
        .palette = LavaColors_p,
    },
    {
        .name = "Cloud",
        .desc = "A flight in the clouds...",
        .palette = CloudColors_p,
    },
    {
        .name = "Party",
        .desc = "A party at the disco...",
        .palette = PartyColors_p,
    },
    {
        .name = "Heat",
        .desc = "It's getting hot in here...",
        .palette = HeatColors_p,
    },
};

uint32_t num_led_palettes() {
    return sizeof(led_palettes) / sizeof(led_palette_t);
}

uint32_t palette_index = 1;
