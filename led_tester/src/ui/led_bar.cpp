#include "led_bar.h"
#include <Arduino.h>

//
// Typedefs
//

//
// Static prototypes
//
static lv_obj_t* led_widget_create(lv_obj_t* parent, int size);
static void led_set_color(lv_obj_t * led, lv_color_t color);
static void delete_array_cb(lv_event_t * e);

//
// Global functions
//
lv_obj_t* led_bar_create(lv_obj_t* parent, uint32_t num_leds) {
    lv_obj_t* led_bar_w = lv_obj_create(parent);
    // The grid descriptor arrays do not get copied by the lvgl library, so
    // they cannot be static or locally scoped.
    // allocate a new array of the size of the number of LEDs plus one for the
    // last template value
    int32_t* grid_col_dsc = new int32_t[num_leds + 1];
    // register a callback for deletion of that array
    lv_obj_add_event_cb(led_bar_w, delete_array_cb, LV_EVENT_DELETE, grid_col_dsc);
    for (uint32_t i = 0; i < num_leds; i++) {
        grid_col_dsc[i] = LV_GRID_FR(1);
    }
    grid_col_dsc[num_leds] = LV_GRID_TEMPLATE_LAST;
    static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(led_bar_w, grid_col_dsc, grid_row_dsc);
    for (uint32_t i = 0; i < num_leds; i++) {
        lv_obj_t* led_w = led_widget_create(led_bar_w, 10);
        lv_obj_set_grid_cell(led_w, LV_GRID_ALIGN_STRETCH, i, 1, LV_GRID_ALIGN_END, 0, 1);
    }
    lv_obj_set_height(led_bar_w, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(led_bar_w, LV_OPA_0, 0);
    lv_obj_set_style_border_opa(led_bar_w, LV_OPA_0, 0);
    lv_obj_set_style_pad_gap(led_bar_w, 3, 0);
    lv_obj_set_style_pad_all(led_bar_w, 1, 0);

    return led_bar_w;
}

//
// Private static functions
//

static lv_obj_t* led_widget_create(lv_obj_t* parent, int size)
{
    // The lvgl LED object has very non-linear way to control the brightness
    // and halo around the widget. So instead we make our own using similar idea
    // (a simple widget with a variable shadow)
    lv_obj_t* led = lv_obj_create(parent);
    lv_obj_set_height(led, size);
    lv_obj_set_width(led, size);
    lv_obj_set_style_pad_all(led, 0, 0);
    lv_obj_set_style_radius(led, size * 2 / 5, 0);
    lv_obj_set_style_border_width(led, 0, 0);
    lv_obj_set_style_shadow_spread(led, size / 8, 0);
    lv_obj_set_style_shadow_width(led, size / 2, 0);
    lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_main_opa(led, LV_OPA_COVER, 0);
    led_set_color(led, lv_color_hex(0x000000));

    return led;
}

static void led_set_color(lv_obj_t * led, lv_color_t color)
{
    // Figure out the overall brightness of the color
    uint32_t brightness = (color.red + color.green + color.blue);
    brightness = brightness > 255 ? 255 : brightness; 
    uint32_t shadow_opacity = brightness < 128 ? 0 : (brightness - 128) * 2;
    lv_obj_set_style_bg_color(led, color, 0);
    lv_obj_set_style_shadow_color(led, color, 0);
    lv_obj_set_style_shadow_opa(led, shadow_opacity, 0);
}

static void delete_array_cb(lv_event_t * e) {
    int32_t* array = (int32_t*) lv_event_get_user_data(e);
    delete[] array;
}

// An LED looking widget
static lv_obj_t* led_widget(lv_obj_t* parent, int size)
{
    // The lvgl LED object has very non-linear way to control the brightness
    // and halo around the widget. So instead we make our own using similar idea
    // (a simple widget with a variable shadow)
    lv_obj_t* led = lv_obj_create(parent);
    lv_obj_set_height(led, size);
    lv_obj_set_width(led, size);
    lv_obj_set_style_pad_all(led, 0, 0);
    lv_obj_set_style_radius(led, size * 2 / 5, 0);
    lv_obj_set_style_border_width(led, 0, 0);
    lv_obj_set_style_shadow_spread(led, size / 8, 0);
    lv_obj_set_style_shadow_width(led, size / 2, 0);
    lv_obj_set_style_bg_opa(led, LV_OPA_COVER, 0);
    lv_obj_set_style_bg_main_opa(led, LV_OPA_COVER, 0);
    led_set_color(led, lv_color_hex(0x000000));

    return led;
}

static void led_set_color(lv_obj_t * led, lv_color_t color)
{
    // Figure out the overall brightness of the color
    uint32_t brightness = (color.red + color.green + color.blue);
    brightness = brightness > 255 ? 255 : brightness; 
    uint32_t shadow_opacity = brightness < 128 ? 0 : (brightness - 128) * 2;
    lv_obj_set_style_bg_color(led, color, 0);
    lv_obj_set_style_shadow_color(led, color, 0);
    lv_obj_set_style_shadow_opa(led, shadow_opacity, 0);
}

