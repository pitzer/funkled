#ifndef NUMBER_INPUT_H
#define NUMBER_INPUT_H

#include "../lvgl.h"

// Create a keyboard widget which will be shared between all the number input widgets
void number_keyboard_init();

// Create a number input widget
lv_obj_t* number_input_create(lv_obj_t* parent, uint32_t* value, uint32_t min, uint32_t max);

// Update the value of a number input widget
void number_input_update(lv_obj_t* number_input_w, uint32_t* value);

#endif // NUMBER_INPUT_H