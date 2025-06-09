#ifndef BRIGHTNESS_SLIDER_H
#define BRIGHTNESS_SLIDER_H

#include <lvgl.h>

// Create a simple slider widget
lv_obj_t* brightness_slider_create(lv_obj_t* parent, lv_event_cb_t slider_pressed_cb, lv_group_t* encoder_group);

#endif // BRIGHTNESS_SLIDER_H