#include <lvgl.h>
#include <Arduino.h>

//
// A small struct to store additional info about the number input
//
typedef struct {
    // This is where the value will be stored
    uint32_t* value;
    // The maximum value allowed
    uint32_t max;
    // The minimum value allowed
    uint32_t min;
} number_input_data_t;

//
// Static variables
//
static lv_obj_t* number_keyboard_w = NULL;

//
// Function prototypes
//
static void number_input_cb(lv_event_t* e);
static void number_input_delete_cb(lv_event_t * e);

//
// Public functions
//
void number_keyboard_init() {
    // A keyboard to fill in numbers
    number_keyboard_w = lv_keyboard_create(lv_screen_active());
    lv_keyboard_set_mode(number_keyboard_w, LV_KEYBOARD_MODE_NUMBER);
    lv_obj_add_flag(number_keyboard_w, LV_OBJ_FLAG_HIDDEN);
}

lv_obj_t* number_input_create(lv_obj_t* parent, uint32_t* value, uint32_t min, uint32_t max) {
    lv_obj_t* number_input_w = lv_textarea_create(parent);
    lv_obj_set_style_text_font(number_input_w, LV_FONT_DEFAULT, 0);
    lv_textarea_set_one_line(number_input_w, true);
    lv_textarea_set_placeholder_text(number_input_w, "0");
    lv_obj_set_style_height(number_input_w, LV_SIZE_CONTENT, 0);
    lv_obj_set_style_width(number_input_w, 70, 0);
    lv_obj_add_event_cb(number_input_w, number_input_cb, LV_EVENT_ALL, NULL);

    // Create a struct with all the internal data
    number_input_data_t* number_data = new number_input_data_t;
    number_data->value = value;
    number_data->max = max;
    number_data->min = min;
    lv_obj_set_user_data(number_input_w, number_data);

    // Register a callback for deletion of the widget and associated storage
    lv_obj_add_event_cb(number_input_w, number_input_delete_cb, LV_EVENT_DELETE, NULL);
    return number_input_w;
}

void number_input_update(lv_obj_t* number_input_w, uint32_t* value) {
    number_input_data_t* number_data = (number_input_data_t*) lv_obj_get_user_data(number_input_w);
    number_data->value = value;
    lv_textarea_set_text(number_input_w, String(*value).c_str());
}

//
// Private callbacks
//
static void number_input_delete_cb(lv_event_t * e) {
    lv_obj_t* number_input_w = (lv_obj_t*) lv_event_get_target(e);
    number_input_data_t* number_data = (number_input_data_t*) lv_obj_get_user_data(number_input_w);
    delete number_data;
}
static void number_input_cb(lv_event_t* e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* text_area_w = (lv_obj_t*) lv_event_get_target(e);
    number_input_data_t* number_data = (number_input_data_t*) lv_obj_get_user_data(text_area_w);
    // Find the top level widget
    lv_obj_t* top_w = lv_obj_get_child(lv_screen_active(), 0);
    if(code == LV_EVENT_FOCUSED) {
        // We just activated the text box. Show the keyboard.
        lv_keyboard_set_textarea(number_keyboard_w, text_area_w);
        lv_obj_set_style_max_height(number_keyboard_w, LV_HOR_RES * 2 / 3, 0);
        lv_obj_update_layout(top_w);
        lv_obj_set_height(top_w, LV_VER_RES - lv_obj_get_height(number_keyboard_w));
        lv_obj_remove_flag(number_keyboard_w, LV_OBJ_FLAG_HIDDEN);
        lv_obj_scroll_to_view_recursive(text_area_w, LV_ANIM_OFF);
        lv_indev_wait_release((lv_indev_t*) lv_event_get_param(e));
        lv_obj_set_style_opa(text_area_w, LV_OPA_100, LV_PART_CURSOR);
    } else if(code == LV_EVENT_DEFOCUSED || code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
        // We just deactivated the text box, pressed OK or Cancel. Hide the keyboard.
        lv_keyboard_set_textarea(number_keyboard_w, NULL);
        lv_obj_set_height(top_w, LV_VER_RES);
        lv_obj_add_flag(number_keyboard_w, LV_OBJ_FLAG_HIDDEN);
        lv_indev_reset(NULL, text_area_w);
        lv_obj_set_style_opa(text_area_w, LV_OPA_0, LV_PART_CURSOR);
        // Now we can update the underlying value
        const char* text = lv_textarea_get_text(text_area_w);
        uint32_t new_value = String(text).toInt();
        if (new_value < number_data->min) {
            new_value = number_data->min;
        }
        if (new_value > number_data->max) {
            new_value = number_data->max;
        }
        *number_data->value = new_value;
        lv_textarea_set_text(text_area_w, String(new_value).c_str());
    }
}