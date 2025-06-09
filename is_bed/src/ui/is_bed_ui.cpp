#include "is_bed_ui.h"
#include "composite_image.h"
#include "color_selector.h"
#include "led_bar.h"
#include "led_pattern.h"
#include "led_palette.h"
#include "led_array.h"
#include "number_input.h"
#include "file_menu.h"
#include "slider.h"
#include <Arduino.h>
#include <FastLED.h>

//
// Constants
//
// The default period to use when displaying patterns
const uint32_t default_period_ms = 3000;
// The default brightness to use when displaying patterns
const uint8_t default_brightness = 255;

//
// Static prototypes
//
static void brightness_changed_cb(lv_event_t* e);

//
// Static variables
//
// Fonts
static const lv_font_t * font_large;
static const lv_font_t * font_normal;
// Styles
static lv_style_t style_text_muted;
static lv_style_t style_title;
static lv_style_t style_list;
static lv_style_t style_list_button;
static lv_style_t style_list_button_checked;
static lv_style_t style_list_button_focused;
// Some widgets we want to keep around for easy access
static lv_obj_t* background_image_w;
static lv_obj_t* cage_brightness_w;
static lv_obj_t* center_brightness_w;
static lv_obj_t* front_brightness_w;
static lv_obj_t* headboard_brightness_w;

// The encoder groups
static lv_group_t * encoder_groups[4];

// The composite image of the bed
LV_IMAGE_DECLARE(background);
LV_IMAGE_DECLARE(cage);
LV_IMAGE_DECLARE(center);
LV_IMAGE_DECLARE(front);
LV_IMAGE_DECLARE(headboard);
static composite_image_layer_t layers[] = {
    {
        .image_dsc = cage,
        .color = lv_color_hex(0x000000)
    }, {
        .image_dsc = center,
        .color = lv_color_hex(0x000000)
    }, {
        .image_dsc = front,
        .color = lv_color_hex(0x000000)
    }, {
        .image_dsc = headboard,
        .color = lv_color_hex(0x000000)
    }
};
static DMAMEM uint8_t composite_buffer[TFT_HOR_RES * TFT_VER_RES * 3];
static composite_image_dsc_t composite_dsc = {
    .buffer = composite_buffer,
    .background_image_dsc = background,
    .layers = layers,
    .layer_count = sizeof(layers) / sizeof(layers[0])
};




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
    // We could assign them to a group and let LVGL handle the link to the sliders,
    // but LVGL will not update the slider value until the encoder is clicked, which is
    // not what we want. So we will handle the input devices ourselves.
    for (lv_indev_t * indev = lv_indev_get_next(NULL); indev != NULL; indev = lv_indev_get_next(indev)) {
        if (lv_indev_get_type(indev) == LV_INDEV_TYPE_ENCODER) {
            // Get the encoder index from the user data
            uint32_t encoder_index = (uint32_t) lv_indev_get_user_data(indev);
            // Add the input device to the corresponding group
            encoder_groups[encoder_index] = lv_group_create();
            lv_indev_set_group(indev, encoder_groups[encoder_index]);
            // Disable wrapping for the group
            lv_group_set_wrap(encoder_groups[encoder_index], false);
        }
    }

    // Styles
    // Muted text
    lv_style_init(&style_text_muted);
    lv_style_set_text_opa(&style_text_muted, LV_OPA_50);
    // Title
    lv_style_init(&style_title);
    lv_style_set_text_font(&style_title, font_large);
    // List
    lv_style_init(&style_list);
    lv_style_set_pad_all(&style_list, 7);
    lv_style_set_pad_gap(&style_list, 7);
    // Button inside a list
    lv_style_init(&style_list_button);
    lv_style_set_border_width(&style_list_button, 0);
    lv_style_set_bg_opa(&style_list_button, 35);
    lv_style_set_text_color(&style_list_button, lv_color_hex(0x202020));
    lv_style_set_pad_all(&style_list_button, 5);
    lv_style_set_pad_gap(&style_list_button, 10);
    lv_style_set_height(&style_list_button, LV_SIZE_CONTENT);
    lv_style_set_width(&style_list_button, LV_PCT(97));

    lv_style_init(&style_list_button_checked);
    lv_style_set_border_width(&style_list_button_checked, 2);
    lv_style_set_bg_opa(&style_list_button_checked, 35);
    lv_style_set_text_color(&style_list_button_checked, lv_color_hex(0x202020));
    lv_style_init(&style_list_button_focused);
    lv_style_set_border_width(&style_list_button_focused, 4);

    background_image_w = composite_image_create(lv_screen_active(), &composite_dsc);

//    lv_obj_t* brightness_label_w = lv_label_create(lv_screen_active());
//    lv_label_set_text(brightness_label_w, "Brightness");
//    lv_obj_add_style(brightness_label_w, &style_text_muted, 0);
//    brightness_value_w = lv_label_create(lv_screen_active());
//    lv_label_set_text(brightness_value_w, "");
    center_brightness_w = slider_create(lv_screen_active(), lv_color_hex(0x000080), brightness_changed_cb, encoder_groups[0]);
    front_brightness_w = slider_create(lv_screen_active(), lv_color_hex(0x000080), brightness_changed_cb, encoder_groups[1]);
    headboard_brightness_w = slider_create(lv_screen_active(), lv_color_hex(0x000080), brightness_changed_cb, encoder_groups[2]);
    cage_brightness_w = slider_create(lv_screen_active(), lv_color_hex(0x000080), brightness_changed_cb, encoder_groups[3]);
    //lv_obj_set_width(brightness_w, 200);
    //lv_obj_set_grid_cell(brightness_label_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    //lv_obj_set_grid_cell(brightness_w, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    //lv_obj_set_grid_cell(brightness_value_w, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

}

//
// Helper functions
//

// A list item widget with an LED bar.
static lv_obj_t* list_item_widget_create(
    lv_obj_t* parent,
    const char* name,
    const char* desc,
    uint32_t channel,
    const led_bar_dsc_t* led_bar_dsc
) {
    // The top button widget
    lv_obj_t * btn_w = lv_btn_create(parent);
    lv_obj_add_style(btn_w, &style_list_button, LV_STATE_DEFAULT);
    lv_obj_add_style(btn_w, &style_list_button_checked, LV_STATE_CHECKED);
    lv_obj_add_style(btn_w, &style_list_button_focused, LV_STATE_FOCUSED);

    // Different layouts if the description is present or not
    if (desc) {
        static int32_t grid_col_dsc[] = {LV_GRID_FR(30), LV_GRID_FR(70), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(btn_w, grid_col_dsc, grid_row_dsc);
    } else {
        static int32_t grid_col_dsc[] = {17, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
        static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
        lv_obj_set_grid_dsc_array(btn_w, grid_col_dsc, grid_row_dsc);
    }

    // The name at the top
    lv_obj_t * name_w = lv_label_create(btn_w);
    lv_label_set_text(name_w, name);
    lv_obj_add_style(name_w, &style_title, 0);
    lv_obj_set_grid_cell(name_w, LV_GRID_ALIGN_STRETCH, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    lv_label_set_long_mode(name_w, LV_LABEL_LONG_WRAP);
    
    // The description
    if (desc) {
        lv_obj_t* desc_w = lv_label_create(btn_w);
        lv_obj_add_style(desc_w, &style_text_muted, 0);
        lv_label_set_text(desc_w, desc);
        lv_obj_set_grid_cell(desc_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_START, 0, 1);
        lv_label_set_long_mode(desc_w, LV_LABEL_LONG_WRAP);
    }

    lv_obj_t* last_panel_w;
    // The led bar
    last_panel_w = led_bar_create(btn_w, 16, channel, led_bar_dsc);
    lv_obj_set_width(last_panel_w, LV_PCT(100));
    if (desc) {
        lv_obj_set_grid_cell(last_panel_w, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_END, 1, 1);
    } else {
        lv_obj_set_grid_cell(last_panel_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_END, 0, 1);
    }

    return btn_w;
}

//
// Callbacks
//

static void brightness_changed_cb(lv_event_t* e) {
    // Get the slider widget that triggered the event
    lv_obj_t* slider_w = (lv_obj_t*) lv_event_get_target(e);
    // Get the new brightness value
    uint32_t brightness = lv_slider_get_value(slider_w);
    lv_group_set_editing(encoder_groups[0], true);
    composite_dsc.layers[0].color.blue = brightness;

    //lv_label_set_text(brightness_value_w, String(brightness).c_str());
    //led_strings[current_channel].brightness = brightness;
}
