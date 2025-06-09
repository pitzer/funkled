
#include "brightness_slider.h"
#include "slider.h"

lv_obj_t* brightness_slider_create(lv_obj_t* parent, lv_event_cb_t slider_pressed_cb, lv_group_t* encoder_group) {
    // Create the slider widget
    lv_obj_t* slider_w = slider_create(parent, lv_color_hex(0x800000), slider_pressed_cb);
    lv_obj_align(slider_w, LV_ALIGN_BOTTOM_MID, 0,  - LV_DPX(20));
    lv_obj_set_height(slider_w, 30);
    lv_obj_set_width(slider_w, LV_PCT(95));
    lv_obj_set_style_pad_right(slider_w, 15, 0);
    lv_obj_set_style_pad_left(slider_w, 15, 0);
    // Register the slider with the encoder group
    lv_group_add_obj(encoder_group, slider_w);
    lv_group_focus_obj(slider_w);
    lv_group_set_editing(encoder_group, true);
    lv_group_focus_freeze(encoder_group, true);
    return slider_w;
}