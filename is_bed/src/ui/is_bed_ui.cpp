#include "is_bed_ui.h"
#include "composite_image.h"
#include "led_pattern.h"
#include "led_palette.h"
#include "led_array.h"
#include "slider.h"
#include "brightness_slider.h"
#include <Arduino.h>
#include <FastLED.h>

//
// Constants
//

//
// Static prototypes
//
static void brightness_changed_cb(lv_event_t *e);

//
// Static variables
//
// Fonts
static const lv_font_t *font_large;
static const lv_font_t *font_normal;
// Some widgets we want to keep around for easy access
static lv_obj_t *background_image_w;
static lv_obj_t *center_brightness_w;
static lv_obj_t *front_brightness_w;
static lv_obj_t *headboard_brightness_w;
static lv_obj_t *cage_brightness_w;

// The encoder groups
static lv_group_t *encoder_groups[4];

// The composite image of the bed
LV_IMAGE_DECLARE(background);
LV_IMAGE_DECLARE(center);
LV_IMAGE_DECLARE(front);
LV_IMAGE_DECLARE(headboard);
LV_IMAGE_DECLARE(cage);
static composite_image_layer_t layers[] = {
    {.image_dsc = center,
     .led_string = &led_strings[0]},
    {.image_dsc = front,
     .led_string = &led_strings[1]},
    {.image_dsc = headboard,
     .led_string = &led_strings[2]},
    {.image_dsc = cage,
     .led_string = &led_strings[3]}};
static DMAMEM uint8_t composite_buffer[TFT_HOR_RES * TFT_VER_RES * 3];
static composite_image_dsc_t composite_dsc = {
    .buffer = composite_buffer,
    .background_image_dsc = background,
    .layers = layers,
    .layer_count = sizeof(layers) / sizeof(layers[0])};

//
// Global functions
//

void is_bed_ui(void)
{
    font_large = &lv_font_montserrat_16;
    font_normal = &lv_font_montserrat_12;
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);

    // Figure out which input devices we have.
    for (lv_indev_t *indev = lv_indev_get_next(NULL); indev != NULL; indev = lv_indev_get_next(indev))
    {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER)
        {
            // Get the encoder index from the user data
            uint32_t encoder_index = (uint32_t)lv_indev_get_user_data(indev);
            // Add the input device to the corresponding group
            encoder_groups[encoder_index] = lv_group_create();
            lv_indev_set_group(indev, encoder_groups[encoder_index]);
            // Disable wrapping for the group
            lv_group_set_wrap(encoder_groups[encoder_index], false);
        }
    }

    // Make sure that the top level screen is not scrollable
    lv_obj_t *screen_w = lv_screen_active();
    lv_obj_clear_flag(screen_w, LV_OBJ_FLAG_SCROLLABLE);

    background_image_w = composite_image_create(screen_w, &composite_dsc);
    center_brightness_w = brightness_slider_create(screen_w, brightness_changed_cb, encoder_groups[0], 0);
    front_brightness_w = brightness_slider_create(screen_w, brightness_changed_cb, encoder_groups[1], 1);
    headboard_brightness_w = brightness_slider_create(screen_w, brightness_changed_cb, encoder_groups[2], 2);
    cage_brightness_w = brightness_slider_create(screen_w, brightness_changed_cb, encoder_groups[3], 3);
}

//
// Callbacks
//
static void brightness_changed_cb(lv_event_t *e)
{
    // Get the slider widget that triggered the event
    lv_obj_t *slider_w = (lv_obj_t *)lv_event_get_target(e);
    // Get the index of the corresponding LEDs from the user data
    uint32_t index = (uint32_t)lv_obj_get_user_data(slider_w);
    // Get the current brightness value from the slider
    uint32_t brightness = lv_slider_get_value(slider_w);
    // When the user presses the button, we want to update the brightness to zero or full
    if (lv_event_get_code(e) == LV_EVENT_KEY && lv_event_get_key(e) == LV_KEY_ENTER)
    {
        if (brightness > 128)
        {
            brightness = 0; // Turn off the LEDs
        }
        else
        {
            brightness = 255; // Turn on the LEDs
        }
        lv_slider_set_value(slider_w, brightness, LV_ANIM_OFF); // Update the slider value
    }
    // Update the corresponding LED string brightness
    led_zones[index].brightness = brightness;
}
