
#include "slider.h"

lv_obj_t* slider_create(lv_obj_t* parent, lv_color_t color, lv_event_cb_t slider_pressed_cb, lv_group_t* encoder_group) {
    // Create the slider widget
    lv_obj_t* slider_w = lv_slider_create(parent);
    lv_obj_set_style_bg_color(slider_w, color, LV_PART_KNOB);
    lv_obj_set_style_bg_color(slider_w, color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_opa(slider_w, LV_OPA_50, LV_PART_INDICATOR);
    lv_obj_set_style_border_width(slider_w, 2, LV_PART_KNOB);
    lv_obj_set_style_border_color(slider_w, lv_color_hex(0x808080), LV_PART_KNOB);
    lv_obj_set_style_pad_right(slider_w, 15, 0);
    lv_obj_set_style_pad_left(slider_w, 15, 0);
    lv_obj_align(slider_w, LV_ALIGN_BOTTOM_MID, 0,  - LV_DPX(20));
    lv_obj_set_style_pad_all(slider_w, 1, LV_PART_KNOB);
    lv_obj_set_height(slider_w, 30);
    lv_obj_set_width(slider_w, LV_PCT(95));
    lv_slider_set_range(slider_w, 0, 255);
    lv_slider_set_value(slider_w, 0, LV_ANIM_OFF);
    // Register the slider with the encoder group if provided
    if (encoder_group) {
        lv_group_add_obj(encoder_group, slider_w);
        lv_group_focus_obj(slider_w);
        lv_group_set_editing(encoder_group, true);
        lv_group_focus_freeze(encoder_group, true);
    }
    // Add event callbacks for pressing, releasing, and key events
    lv_obj_add_event_cb(slider_w, slider_pressed_cb, LV_EVENT_PRESSING, parent);
    lv_obj_add_event_cb(slider_w, slider_pressed_cb, LV_EVENT_RELEASED, parent);
    lv_obj_add_event_cb(slider_w, slider_pressed_cb, LV_EVENT_KEY, parent);
    return slider_w;
}