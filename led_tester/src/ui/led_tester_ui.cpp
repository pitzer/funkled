#include "led_tester_ui.h"
#include "color_selector.h"
#include "led_bar.h"
#include <Arduino.h>

//
// Constants
//
// The maximum number of patterns we can show
static const int MAX_PATTERNS = 64;

//
// Typedefs
//
typedef enum {
    PATTERN_FADE,
    PATTERN_BLINK,
} pattern_type_t;

//
// Static prototypes
//
static lv_obj_t* pattern_widget_create(lv_obj_t* parent, pattern_type_t type, const char* title, const char* desc);
static lv_obj_t* led_widget(lv_obj_t* parent, int size);
static void led_set_color(lv_obj_t* led, lv_color_t color);
static void leds_timer_cb(lv_timer_t* timer);

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
// Tab views
static lv_obj_t * tv;
// The LEDs that we can control on a pattern
static int pattern_count = 0;
//static led_t leds[MAX_PATTERNS];

//
// Global functions
//

void led_tester_ui(void)
{
    font_large = &lv_font_montserrat_18;
    font_normal = &lv_font_montserrat_14;
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
    lv_style_set_pad_all(&style_list_button, 10);
    lv_style_set_pad_gap(&style_list_button, 10);

    tv = lv_tabview_create(lv_screen_active());
    lv_tabview_set_tab_bar_size(tv, tab_h);

    lv_obj_set_style_text_font(lv_screen_active(), font_normal, 0);


    // First tab: patterns
    String pattern_str = String(LV_SYMBOL_LOOP) + " Pattern";
    lv_obj_t* t1_w = lv_tabview_add_tab(tv, pattern_str.c_str());
    lv_obj_set_flex_flow(t1_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(t1_w, &style_list, 0);
    pattern_widget_create(t1_w, PATTERN_FADE, "Fade", "Fade in and out and do plenty of stuff and this should now wrap");
    pattern_widget_create(t1_w, PATTERN_BLINK, "Blink", "Blink on and off");
    pattern_widget_create(t1_w, PATTERN_FADE, "Fade", "Fade in and out");
    pattern_widget_create(t1_w, PATTERN_BLINK, "Blink", "Blink on and off");

   // Second tab: colors
    String color_str = String(LV_SYMBOL_TINT) + " Color";
    lv_obj_t* t2_w = lv_tabview_add_tab(tv, color_str.c_str());
    lv_obj_set_flex_flow(t2_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(t2_w, &style_list, 0);
    lv_obj_t* color_selector_w = color_selector_create(t2_w);
    lv_obj_add_style(color_selector_w, &style_list_button, 0);    
    lv_obj_set_height(color_selector_w, LV_SIZE_CONTENT);
    lv_obj_set_width(color_selector_w, LV_PCT(97));

    // third tab: system
    String system_str = String(LV_SYMBOL_POWER) + " System";
    lv_obj_t* t3_w = lv_tabview_add_tab(tv, system_str.c_str());
    lv_obj_set_flex_flow(t3_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_t* led_bar_w = led_bar_create(t3_w, 16);
    lv_obj_set_width(led_bar_w, LV_PCT(97));

    // fourth tab: options
    String options_str = String(LV_SYMBOL_SETTINGS) + " Params";
    lv_obj_t* t4_w = lv_tabview_add_tab(tv, options_str.c_str());
    lv_obj_set_flex_flow(t4_w, LV_FLEX_FLOW_COLUMN);



    // Create a timer for the LED patterns
    lv_timer_create(leds_timer_cb, 15, NULL);
}

//
// Widget creators
//

// A pattern descriptor widget
static lv_obj_t* pattern_widget_create(lv_obj_t* parent, pattern_type_t type, const char* name, const char* desc)
{
    // If we exhausted the max number of patterns, just bail out
    if (pattern_count >= MAX_PATTERNS) {
        return NULL;
    }

    // The top button widget
    lv_obj_t * btn_w = lv_btn_create(parent);
    lv_obj_add_style(btn_w, &style_list_button, 0);    
    lv_obj_set_height(btn_w, LV_SIZE_CONTENT);
    lv_obj_set_width(btn_w, LV_PCT(97));
    static int32_t grid_col_dsc[] = {LV_GRID_FR(30), LV_GRID_FR(70), LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(btn_w, grid_col_dsc, grid_row_dsc);

    // The name at the top
    lv_obj_t * name_w = lv_label_create(btn_w);
    lv_label_set_text(name_w, name);
    lv_obj_add_style(name_w, &style_title, 0);
    lv_obj_set_grid_cell(name_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    
    // The led bar
    lv_obj_t * led_bar_w = led_bar_create(btn_w, 12);
    lv_obj_set_width(led_bar_w, LV_PCT(100));
    lv_obj_set_grid_cell(led_bar_w, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_END, 1, 1);

    // The description
    lv_obj_t * desc_w = lv_label_create(btn_w);
    lv_obj_add_style(desc_w, &style_text_muted, 0);
    lv_label_set_text(desc_w, desc);
    lv_obj_set_grid_cell(desc_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_long_mode(desc_w, LV_LABEL_LONG_WRAP);


    pattern_count++;

    return btn_w;
}

//
// Private callbacks
//
static void leds_timer_cb(lv_timer_t * timer)
{
    LV_UNUSED(timer);

    #if 0

    return;
    uint32_t t = millis();
    uint32_t period = 750;

    for (int i = 0; i < pattern_count; i++) {
        led_t * led = &leds[i];
        for (int j = 0; j < LED_WIDGET_COUNT; j++) {
            uint32_t bright = 0;
            uint32_t phase = ((t % period) * 256 / period + j * 256 / LED_WIDGET_COUNT) % 256;
            switch (led->type) {
                case PATTERN_FADE:
                    bright = phase < 128 ? phase * 2 : 255 - (phase - 128) * 2;
                    break;
                case PATTERN_BLINK:
                    bright = phase < 128 ? 255 : 0;
                    break;
            }
            lv_color_t color = lv_color_mix(lv_palette_main(LV_PALETTE_RED), lv_color_black(), bright <= 255 ? bright : 255);
            led_set_color(led->leds[j], color);
        }
    }
    #endif
}
