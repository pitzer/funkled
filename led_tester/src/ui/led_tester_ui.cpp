#include "led_tester_ui.h"
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
static lv_obj_t* list_item_widget_create(
    lv_obj_t* parent,
    const char* name,
    const char* desc,
    uint32_t channel,
    const led_pattern_func_t pattern_update,
    const CRGBPalette16* palette,
    const uint32_t* period_ms,
    const uint8_t* brightness,
    bool use_color_selector
);
static void pattern_clicked_cb(lv_event_t* e);
static void palette_clicked_cb(lv_event_t* e);
static void channel_clicked_cb(lv_event_t* e);
static void brightness_pressed_cb(lv_event_t* e);
static void color_order_changed_cb(lv_event_t* e);
static void file_menu_cb(uint32_t index);

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
// Tabs
static lv_obj_t* tabview_w;
static lv_obj_t* tab_pattern_w;
static lv_obj_t* tab_color_w;
static lv_obj_t* tab_channel_w;
static lv_obj_t* tab_params_w;
// Some widgets we want to keep around for easy access
static lv_obj_t* channel_label_w;
static lv_obj_t* num_leds_w;
static lv_obj_t* period_w;
static lv_obj_t* brightness_w;
static lv_obj_t* brightness_value_w;
static lv_obj_t* color_order_w;

//
// Global functions
//

void led_tester_ui(void)
{
    font_large = &lv_font_montserrat_16;
    font_normal = &lv_font_montserrat_12;
    int32_t tab_h = 35;
    lv_theme_default_init(NULL, lv_palette_main(LV_PALETTE_BLUE), lv_palette_main(LV_PALETTE_RED), LV_THEME_DEFAULT_DARK,
                          font_normal);

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

    // Create the top level tab view
    tabview_w = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tabview_w, tab_h);
    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);

    // A keyboard to fill in numbers
    number_keyboard_init();

    // First tab: channel
    String system_str = String(LV_SYMBOL_LIST) + " Channel";
    tab_channel_w = lv_tabview_add_tab(tabview_w, system_str.c_str());
    lv_obj_set_flex_flow(tab_channel_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(tab_channel_w, &style_list, 0);
    for (uint32_t i = 0; i < num_led_channels; i++) {
        lv_obj_t* channel_w = list_item_widget_create(
            tab_channel_w,
            String(i).c_str(),
            NULL,
            i,
            NULL,
            NULL,
            NULL,
            NULL,
            false);
        lv_obj_add_event_cb(channel_w, channel_clicked_cb, LV_EVENT_CLICKED, (void*) i);
    }

    // Second tab: patterns
    String pattern_str = String(LV_SYMBOL_LOOP) + " Pattern";
    tab_pattern_w = lv_tabview_add_tab(tabview_w, pattern_str.c_str());
    lv_obj_set_flex_flow(tab_pattern_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(tab_pattern_w, &style_list, 0);
    for (uint32_t i = 0; i < num_led_patterns(); i++) {
        led_pattern_t* pattern = &led_patterns[i];
        lv_obj_t* pattern_w = list_item_widget_create(
            tab_pattern_w,
            pattern->name,
            pattern->desc,
            CHANNEL_CURRENT,
            pattern->update,
            NULL,
            &default_period_ms,
            &default_brightness,
            false);
        lv_obj_add_event_cb(pattern_w, pattern_clicked_cb, LV_EVENT_CLICKED, (void*) i);
    }

    // Third tab: colors
    String color_str = String(LV_SYMBOL_TINT) + " Color";
    tab_color_w = lv_tabview_add_tab(tabview_w, color_str.c_str());
    lv_obj_set_flex_flow(tab_color_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(tab_color_w, &style_list, 0);
    for (uint32_t i = 0; i < num_led_palettes(); i++) {
        led_palette_t* palette = &led_palettes[i];
        // The first palette is the solid color one.
        // Add a color selector to be able to change it.
        lv_obj_t* palette_w = list_item_widget_create(
            tab_color_w,
            palette->name,
            palette->desc,
            CHANNEL_CURRENT,
            NULL,
            &palette->palette,
            &default_period_ms,
            &default_brightness,
            String(palette->name) == "Solid");
        lv_obj_add_event_cb(palette_w, palette_clicked_cb, LV_EVENT_CLICKED, (void*) i);
    }

    // Fourth tab: parameters
    String params_str = String(LV_SYMBOL_SETTINGS) + " Params";
    tab_params_w = lv_tabview_add_tab(tabview_w, params_str.c_str());
    lv_obj_add_style(tab_params_w, &style_list, 0);
    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(tab_params_w, grid_col_dsc, grid_row_dsc);
    // Title
    channel_label_w = lv_label_create(tab_params_w);
    lv_label_set_text(channel_label_w, "Channel");
    lv_obj_add_style(channel_label_w, &style_title, 0);
    lv_obj_set_grid_cell(channel_label_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    // LED bar
    lv_obj_t* led_bar_w = led_bar_create(tab_params_w, 12, CHANNEL_CURRENT, NULL, NULL, NULL, NULL);
    lv_obj_set_grid_cell(led_bar_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    // Number of LEDs
    lv_obj_t* num_leds_label_w = lv_label_create(tab_params_w);
    lv_label_set_text(num_leds_label_w, "Number of LEDs");
    lv_obj_add_style(num_leds_label_w, &style_text_muted, 0);
    num_leds_w = number_input_create(tab_params_w, &led_strings[current_channel].num_leds, 1, max_leds_per_channel);
    lv_obj_set_grid_cell(num_leds_label_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_set_grid_cell(num_leds_w, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    // Period
    lv_obj_t* period_label_w = lv_label_create(tab_params_w);
    lv_label_set_text(period_label_w, "Period (ms)");
    lv_obj_add_style(period_label_w, &style_text_muted, 0);
    period_w = number_input_create(tab_params_w, &led_strings[current_channel].update_period_ms, 100, 10000);
    lv_obj_set_grid_cell(period_label_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_set_grid_cell(period_w, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    // Color ordering
    lv_obj_t* color_order_label_w = lv_label_create(tab_params_w);
    lv_label_set_text(color_order_label_w, "Color Order");
    lv_obj_add_style(color_order_label_w, &style_text_muted, 0);
    color_order_w = lv_dropdown_create(tab_params_w);
    lv_dropdown_set_options(color_order_w, "RGB\nRBG\nGRB\nGBR\nBRG\nBGR\n");
    lv_obj_set_width(color_order_w, 70);
    lv_obj_add_event_cb(color_order_w, color_order_changed_cb, LV_EVENT_VALUE_CHANGED, NULL);
    lv_obj_set_grid_cell(color_order_label_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    lv_obj_set_grid_cell(color_order_w, LV_GRID_ALIGN_START, 1, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    // Brightness
    lv_obj_t* brightness_label_w = lv_label_create(tab_params_w);
    lv_label_set_text(brightness_label_w, "Brightness");
    lv_obj_add_style(brightness_label_w, &style_text_muted, 0);
    brightness_value_w = lv_label_create(tab_params_w);
    lv_label_set_text(brightness_value_w, "");
    brightness_w = slider_create(tab_params_w, lv_color_hex(0x000080), brightness_pressed_cb);
    lv_obj_set_grid_cell(brightness_label_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(brightness_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);
    lv_obj_set_grid_cell(brightness_value_w, LV_GRID_ALIGN_CENTER, 1, 1, LV_GRID_ALIGN_CENTER, 4, 1);

    // The file menu
    static const char* file_menu_items[] = {LV_SYMBOL_DOWNLOAD, LV_SYMBOL_UPLOAD, LV_SYMBOL_HOME, NULL};
    file_menu_create(tabview_w, file_menu_items, file_menu_cb);

    // Attempt to load the data from the EEPROM
    led_array_load();

    // Select the first channel
    lv_obj_send_event(lv_obj_get_child(tab_channel_w, 0), LV_EVENT_CLICKED, NULL);
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
    const led_pattern_func_t pattern_update,
    const CRGBPalette16* palette,
    const uint32_t* period_ms,
    const uint8_t* brightness,
    bool use_color_selector
) {
    // The top button widget
    lv_obj_t * btn_w = lv_btn_create(parent);
    lv_obj_add_style(btn_w, &style_list_button, LV_STATE_DEFAULT);
    lv_obj_add_style(btn_w, &style_list_button_checked, LV_STATE_CHECKED);
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
    if (use_color_selector) {
        // The color selector
        last_panel_w = color_selector_create(btn_w);
        lv_obj_set_style_bg_opa(last_panel_w, LV_OPA_0, 0);
        lv_obj_set_style_pad_all(last_panel_w, 0, 0);
        lv_obj_set_style_pad_right(last_panel_w, 10, 0);
        lv_obj_set_style_border_opa(last_panel_w, LV_OPA_0, 0);
        lv_obj_set_style_shadow_opa(last_panel_w, LV_OPA_0, 0);
    } else {
        // The led bar
        last_panel_w = led_bar_create(btn_w, 16, channel, pattern_update, palette, period_ms, brightness);
    }
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

static void pattern_clicked_cb(lv_event_t* e) {
    // Get the new pattern index
    uint32_t pattern_index = (uint32_t) lv_event_get_user_data(e);
    led_strings[current_channel].pattern_index = pattern_index;
    // Unclick all the other buttons
    for (uint32_t i = 0; i < lv_obj_get_child_count(tab_pattern_w); i++) {
        lv_obj_t* btn_w = lv_obj_get_child(tab_pattern_w, i);
        if (i != pattern_index) {
            lv_obj_clear_state(btn_w, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(btn_w, LV_STATE_CHECKED);
        }
    }
}

static void palette_clicked_cb(lv_event_t* e) {
    // Get the new palette index
    uint32_t palette_index = (uint32_t) lv_event_get_user_data(e);
    led_strings[current_channel].palette_index = palette_index;
    // Unclick all the other buttons
    for (uint32_t i = 0; i < lv_obj_get_child_count(tab_color_w); i++) {
        lv_obj_t* btn_w = lv_obj_get_child(tab_color_w, i);
        if (i != palette_index) {
            lv_obj_clear_state(btn_w, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(btn_w, LV_STATE_CHECKED);
        }
    }
    // If the solid palette is selected, update its color
    const char* name = led_palettes[palette_index].name;
    if (String(name) == "Solid") {
        lv_obj_t* btn_w = lv_obj_get_child(tab_color_w, palette_index);
        lv_obj_t* color_selector_w = lv_obj_get_child(btn_w, 2);
        lv_obj_t* patch_w = lv_obj_get_child(color_selector_w, 1);
        lv_color_t color_lv = lv_obj_get_style_bg_color(patch_w, 0);
        CRGB color_crgb = CRGB(color_lv.red, color_lv.green, color_lv.blue);
        led_palettes[palette_index].palette = CRGBPalette16(color_crgb);
    }

}

static void channel_clicked_cb(lv_event_t* e) {
    // Get the new channel index
    uint32_t channel = (uint32_t) lv_event_get_user_data(e);
    current_channel = channel;
    // Unclick all the other buttons
    for (uint32_t i = 0; i < lv_obj_get_child_count(tab_channel_w); i++) {
        lv_obj_t* btn_w = lv_obj_get_child(tab_channel_w, i);
        if (i != channel) {
            lv_obj_clear_state(btn_w, LV_STATE_CHECKED);
        } else {
            lv_obj_add_state(btn_w, LV_STATE_CHECKED);
        }
    }
    // Update the pattern and palette buttons
    lv_obj_t* pattern_w = lv_obj_get_child(tab_pattern_w, led_strings[channel].pattern_index);
    lv_obj_t* palette_w = lv_obj_get_child(tab_color_w, led_strings[channel].palette_index);
    lv_obj_send_event(pattern_w, LV_EVENT_CLICKED, NULL);
    lv_obj_send_event(palette_w, LV_EVENT_CLICKED, NULL);
    // Update the parameters
    number_input_update(num_leds_w, &led_strings[channel].num_leds);
    number_input_update(period_w, &led_strings[channel].update_period_ms);
    lv_label_set_text(channel_label_w, ("Channel " + String(channel)).c_str());
    lv_slider_set_value(brightness_w, led_strings[channel].brightness, LV_ANIM_OFF);
    lv_label_set_text(brightness_value_w, String(led_strings[channel].brightness).c_str());
    lv_dropdown_set_selected(color_order_w, led_strings[channel].color_order);
}

static void brightness_pressed_cb(lv_event_t* e) {
    // Get the new brightness value
    uint32_t brightness = lv_slider_get_value(brightness_w);
    lv_label_set_text(brightness_value_w, String(brightness).c_str());
    led_strings[current_channel].brightness = brightness;
}

static void color_order_changed_cb(lv_event_t* e) {
    // Get the new order value
    uint32_t order = lv_dropdown_get_selected(color_order_w);
    led_strings[current_channel].color_order = order;
}

static void file_menu_cb(uint32_t index) {
    switch (index) {
        case 0:
            // Save the current configuration
            led_array_save();
            break;
        case 1:
            // Load the saved configuration
            led_array_load();
            break;
        case 2:
            // Reset the led array
            led_array_init();
            // Select the first channel
            lv_obj_send_event(lv_obj_get_child(tab_channel_w, 0), LV_EVENT_CLICKED, NULL);
            break;
    }
}