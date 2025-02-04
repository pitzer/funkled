#include "color_selector.h"
#include <Arduino.h>

//
// Typedefs
//

//
// Static prototypes
//
static void colorwheel_pressed_cb(lv_event_t * e);
static void slider_pressed_cb(lv_event_t * e);
static lv_obj_t* slider_create(lv_obj_t* parent, lv_color_t color);
static void change_color(lv_obj_t* color_selector_w, lv_color_t color);

//
// Global functions
//
lv_obj_t* color_selector_create(lv_obj_t* parent) {
    lv_obj_t* color_selector_w = lv_btn_create(parent);
    static int32_t grid_col_dsc[] = {LV_GRID_CONTENT, 7, LV_GRID_FR(1), 7, LV_GRID_TEMPLATE_LAST};
    static int32_t grid_row_dsc[] = {LV_GRID_CONTENT, 30, 30, 30, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(color_selector_w, grid_col_dsc, grid_row_dsc);
    lv_obj_add_flag(color_selector_w, LV_OBJ_FLAG_EVENT_BUBBLE);
    // The color wheel
    LV_IMAGE_DECLARE(img_colorwheel); 
    lv_obj_t* image_w = lv_image_create(color_selector_w);
    lv_image_set_src(image_w, &img_colorwheel);
    lv_obj_set_grid_cell(image_w, LV_GRID_ALIGN_CENTER, 0, 1, LV_GRID_ALIGN_START, 0, 4);
    lv_obj_add_event_cb(color_selector_w, colorwheel_pressed_cb, LV_EVENT_PRESSING, color_selector_w);
    lv_obj_add_flag(image_w, LV_OBJ_FLAG_EVENT_BUBBLE);
    // The color patch
    lv_obj_t* patch_w = lv_obj_create(color_selector_w);
    lv_obj_t* patch_label_w = lv_label_create(patch_w);
    lv_obj_align(patch_label_w, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_scrollbar_mode(patch_w, LV_SCROLLBAR_MODE_OFF);
    lv_obj_set_style_text_align(patch_w, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_height(patch_w, 30);
    lv_obj_set_style_radius(patch_w, 10, 0);
    lv_obj_set_grid_cell(patch_w, LV_GRID_ALIGN_STRETCH, 1, 3, LV_GRID_ALIGN_CENTER, 0, 1);
    lv_obj_clear_flag(patch_w, LV_OBJ_FLAG_CLICKABLE);
    // The three sliders
    lv_obj_t* red_slider_w = slider_create(color_selector_w, lv_color_hex(0xFF0000));
    lv_obj_set_grid_cell(red_slider_w, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 1, 1);
    lv_obj_t* green_slider_w = slider_create(color_selector_w, lv_color_hex(0x00FF00));
    lv_obj_set_grid_cell(green_slider_w, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 2, 1);
    lv_obj_t* blue_slider_w = slider_create(color_selector_w, lv_color_hex(0x0000FF));
    lv_obj_set_grid_cell(blue_slider_w, LV_GRID_ALIGN_STRETCH, 2, 1, LV_GRID_ALIGN_CENTER, 3, 1);
    // Initial color set
    change_color(color_selector_w, lv_color_hex(0xFFFFFF));

    return color_selector_w;
}

//
// Private callbacks
//
static void colorwheel_pressed_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSING) {
        lv_indev_t * indev = lv_indev_active();
        if(indev == NULL) return;
        lv_indev_type_t indev_type = lv_indev_get_type(indev);
        if(indev_type != LV_INDEV_TYPE_POINTER) return;
        lv_point_t p;
        lv_indev_get_point(indev, &p);
        // Get coordinates relative to the center of the image
        lv_obj_t* color_selector_w = (lv_obj_t*) lv_event_get_user_data(e);
        lv_obj_t* image_w = lv_obj_get_child(color_selector_w, 0);
        lv_area_t image_coords;
        lv_obj_get_coords(image_w, &image_coords);
        p.x -= image_coords.x1;
        p.y -= image_coords.y1;
        // Get the pixel color and alpha
        lv_image_dsc_t* image_dcs = (lv_image_dsc_t*) lv_image_get_src(image_w);
        if (image_dcs->header.cf != LV_COLOR_FORMAT_RGB565A8) {
            return;
        }
        if (p.x < 0 || p.y < 0 || p.x >= image_dcs->header.w || p.y >= image_dcs->header.h) {
            return;
        }
        uint32_t offset_rgb = (p.y * image_dcs->header.w + p.x) * 2;
        uint32_t offset_alpha = image_dcs->header.w * image_dcs->header.h * 2 + p.y * image_dcs->header.w + p.x;
        uint8_t alpha = image_dcs->data[offset_alpha];
        if (alpha == 0) {
            return;
        }
        uint16_t rgb = image_dcs->data[offset_rgb] | (image_dcs->data[offset_rgb + 1] << 8);
        uint8_t r = (rgb & 0xF800) >> 8;
        uint8_t g = (rgb & 0x07E0) >> 3;
        uint8_t b = (rgb & 0x001F) << 3;
        lv_color_t color = lv_color_make(r, g, b);
        change_color(color_selector_w, color);
        return;
    }
}

static void slider_pressed_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    if (code == LV_EVENT_PRESSING) {
        lv_obj_t* color_selector_w = (lv_obj_t*) lv_event_get_user_data(e);
        lv_obj_t* red_slider_w = lv_obj_get_child(color_selector_w, 2);
        lv_obj_t* green_slider_w = lv_obj_get_child(color_selector_w, 3);
        lv_obj_t* blue_slider_w = lv_obj_get_child(color_selector_w, 4);
        lv_color_t color = lv_color_make(lv_slider_get_value(red_slider_w), lv_slider_get_value(green_slider_w), lv_slider_get_value(blue_slider_w));
        change_color(color_selector_w, color);
        return;
    }
}

static void change_color(lv_obj_t* color_selector_w, lv_color_t color) {
    lv_obj_t* patch_w = lv_obj_get_child(color_selector_w, 1);
    lv_obj_t* patch_label_w = lv_obj_get_child(patch_w, 0);
    lv_obj_t* red_slider_w = lv_obj_get_child(color_selector_w, 2);
    lv_obj_t* green_slider_w = lv_obj_get_child(color_selector_w, 3);
    lv_obj_t* blue_slider_w = lv_obj_get_child(color_selector_w, 4);
    char color_str[9];
    snprintf(color_str, 9, "0x%02X%02X%02X", color.red, color.green, color.blue);
    if (color.red + color.green + color.blue < 200) {
        lv_obj_set_style_text_color(patch_label_w, lv_color_hex(0xFFFFFF), 0);
    } else {
        lv_obj_set_style_text_color(patch_label_w, lv_color_hex(0x000000), 0);
    }
    lv_label_set_text(patch_label_w, color_str);
    lv_obj_set_style_bg_color(patch_w, color, 0);
    lv_slider_set_value(red_slider_w, color.red, LV_ANIM_ON);
    lv_slider_set_value(green_slider_w, color.green, LV_ANIM_ON);
    lv_slider_set_value(blue_slider_w, color.blue, LV_ANIM_ON);
}


static lv_obj_t* slider_create(lv_obj_t* parent, lv_color_t color) {
    lv_obj_t* slider_w = lv_slider_create(parent);
    lv_obj_set_style_bg_color(slider_w, color, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider_w, color, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(slider_w, 2, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider_w, lv_color_hex(0x808080), LV_PART_KNOB);
    lv_obj_set_style_pad_all(slider_w, 1, LV_PART_KNOB);
    lv_obj_set_height(slider_w, 20);
    lv_slider_set_range(slider_w, 0, 255);
    lv_slider_set_value(slider_w, 0, LV_ANIM_OFF);
    lv_obj_add_event_cb(slider_w, slider_pressed_cb, LV_EVENT_PRESSING, parent);
    lv_obj_add_flag(slider_w, LV_OBJ_FLAG_EVENT_BUBBLE);
    return slider_w;
}