#include "led_tester_ui.h"
#include "color_selector.h"
#include "led_bar.h"
#include "led_pattern.h"
#include "led_palette.h"
#include <Arduino.h>
#include <FastLED.h>

//
// Typedefs
//
//
// Static prototypes
//
static lv_obj_t* pattern_widget_create(lv_obj_t* parent, led_pattern_t* patttern);
static lv_obj_t* palette_widget_create(lv_obj_t* parent, led_palette_t* palette);

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
    for (uint32_t i = 0; i < num_led_patterns(); i++) {
        led_pattern_t* pattern = &led_patterns[i];
        pattern_widget_create(t1_w, pattern);
    }

    // Second tab: colors
    String color_str = String(LV_SYMBOL_TINT) + " Color";
    lv_obj_t* t2_w = lv_tabview_add_tab(tv, color_str.c_str());
    lv_obj_set_flex_flow(t2_w, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_style(t2_w, &style_list, 0);
    for (uint32_t i = 0; i < num_led_palettes(); i++) {
        palette_widget_create(t2_w, &led_palettes[i]);
    }

    //lv_obj_t* color_selector_w = color_selector_create(t2_w);
    //lv_obj_add_style(color_selector_w, &style_list_button, 0);    
    //lv_obj_set_height(color_selector_w, LV_SIZE_CONTENT);
    //lv_obj_set_width(color_selector_w, LV_PCT(97));

    // third tab: system
    String system_str = String(LV_SYMBOL_POWER) + " System";
    lv_obj_t* t3_w = lv_tabview_add_tab(tv, system_str.c_str());
    lv_obj_set_flex_flow(t3_w, LV_FLEX_FLOW_COLUMN);
    //lv_obj_t* led_bar_w = led_bar_create(t3_w, 16, NULL);
    //lv_obj_set_width(led_bar_w, LV_PCT(97));

    // fourth tab: options
    String options_str = String(LV_SYMBOL_SETTINGS) + " Params";
    lv_obj_t* t4_w = lv_tabview_add_tab(tv, options_str.c_str());
    lv_obj_set_flex_flow(t4_w, LV_FLEX_FLOW_COLUMN);



}

//
// Widget creators
//

// A pattern descriptor widget
static lv_obj_t* pattern_widget_create(lv_obj_t* parent, led_pattern_t* pattern)
{
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
    lv_label_set_text(name_w, pattern->name);
    lv_obj_add_style(name_w, &style_title, 0);
    lv_obj_set_grid_cell(name_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    
    // The led bar
    lv_obj_t * led_bar_w = led_bar_create(btn_w, 16, pattern, current_palette(), 1000);
    lv_obj_set_width(led_bar_w, LV_PCT(100));
    lv_obj_set_grid_cell(led_bar_w, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_END, 1, 1);

    // The description
    lv_obj_t * desc_w = lv_label_create(btn_w);
    lv_obj_add_style(desc_w, &style_text_muted, 0);
    lv_label_set_text(desc_w, pattern->desc);
    lv_obj_set_grid_cell(desc_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_long_mode(desc_w, LV_LABEL_LONG_WRAP);

    return btn_w;
}

// A palette descriptor widget
static lv_obj_t* palette_widget_create(lv_obj_t* parent, led_palette_t* palette)
{
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
    lv_label_set_text(name_w, palette->name);
    lv_obj_add_style(name_w, &style_title, 0);
    lv_obj_set_grid_cell(name_w, LV_GRID_ALIGN_START, 0, 1, LV_GRID_ALIGN_START, 0, 1);
    
    // The led bar
    lv_obj_t * led_bar_w = led_bar_create(btn_w, 16, current_pattern(), &palette->palette, 1000);
    lv_obj_set_width(led_bar_w, LV_PCT(100));
    lv_obj_set_grid_cell(led_bar_w, LV_GRID_ALIGN_STRETCH, 0, 2, LV_GRID_ALIGN_END, 1, 1);

    // The description
    lv_obj_t * desc_w = lv_label_create(btn_w);
    lv_obj_add_style(desc_w, &style_text_muted, 0);
    lv_label_set_text(desc_w, palette->desc);
    lv_obj_set_grid_cell(desc_w, LV_GRID_ALIGN_STRETCH, 1, 1, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_label_set_long_mode(desc_w, LV_LABEL_LONG_WRAP);

    return btn_w;
}
