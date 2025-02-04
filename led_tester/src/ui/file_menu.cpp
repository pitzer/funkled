#include "file_menu.h"

static lv_obj_t* menu_item_create(lv_obj_t* parent, uint32_t index, const char* image, file_menu_cb_t cb);
static void menu_item_event_cb(lv_event_t * e);
static void file_menu_anim_cb(void * var, int32_t v);
static void file_menu_event_cb(lv_event_t * e);


lv_obj_t* file_menu_create(lv_obj_t * parent, const char** items, file_menu_cb_t cb) {
    lv_obj_t * menu_cont_w = lv_obj_create(parent);
    lv_obj_remove_style_all(menu_cont_w);
    lv_obj_set_flex_flow(menu_cont_w, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(menu_cont_w, LV_FLEX_ALIGN_SPACE_EVENLY, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(menu_cont_w, LV_OBJ_FLAG_FLOATING);
    lv_obj_set_style_bg_color(menu_cont_w, lv_color_white(), 0);
    lv_obj_set_style_pad_right(menu_cont_w, LV_DPX(47), 0);
    lv_obj_set_style_bg_opa(menu_cont_w, LV_OPA_COVER, 0);
    lv_obj_set_style_radius(menu_cont_w, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_size(menu_cont_w, LV_DPX(52), LV_DPX(52));
    lv_obj_align(menu_cont_w, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));

    for (uint32_t i = 0; items[i] != NULL; i++) {
        menu_item_create(menu_cont_w, i, items[i], cb);
    }

    lv_obj_t * menu_btn_w = lv_button_create(parent);
    lv_obj_add_flag(menu_btn_w, (lv_obj_flag_t) (LV_OBJ_FLAG_FLOATING | LV_OBJ_FLAG_CLICKABLE));
    lv_obj_set_style_bg_color(menu_btn_w, lv_color_white(), LV_STATE_CHECKED);
    lv_obj_set_style_pad_all(menu_btn_w, 10, 0);
    lv_obj_set_style_radius(menu_btn_w, LV_RADIUS_CIRCLE, 0);
    lv_obj_add_event_cb(menu_btn_w, file_menu_event_cb, LV_EVENT_ALL, menu_cont_w);
    lv_obj_set_style_shadow_width(menu_btn_w, 0, 0);
    lv_obj_set_style_bg_image_src(menu_btn_w, LV_SYMBOL_FILE, 0);
    lv_obj_set_style_text_font(menu_btn_w, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(menu_btn_w, lv_color_hex(0x202020), 0);
    lv_obj_set_size(menu_btn_w, LV_DPX(42), LV_DPX(42));
    lv_obj_align(menu_btn_w, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(15), - LV_DPX(15));

    return menu_cont_w;
}

lv_obj_t* menu_item_create(lv_obj_t* parent, uint32_t index, const char* image, file_menu_cb_t cb) {
    lv_obj_t* item_w = lv_button_create(parent);
    lv_obj_set_style_bg_opa(item_w, 35, 0);
    lv_obj_set_style_radius(item_w, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_opa(item_w, LV_OPA_TRANSP, 0);
    lv_obj_set_style_bg_image_src(item_w, image, 0);
    lv_obj_set_size(item_w, LV_DPX(42), LV_DPX(42));
    lv_obj_set_style_text_font(item_w, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(item_w, lv_color_hex(0x202020), 0);
    lv_obj_set_user_data(item_w, (void*) index);
    lv_obj_add_event_cb(item_w, menu_item_event_cb, LV_EVENT_ALL, (void*) cb);
    lv_obj_remove_flag(item_w, LV_OBJ_FLAG_SCROLL_ON_FOCUS);

    return item_w;
}

void menu_item_event_cb(lv_event_t * e) {
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t* obj = (lv_obj_t*) lv_event_get_target(e);

    if(code == LV_EVENT_FOCUSED) {
        lv_obj_t * menu_cont_w = lv_obj_get_parent(obj);
        if(lv_obj_get_width(menu_cont_w) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, menu_cont_w);
            lv_anim_set_exec_cb(&a, file_menu_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
    }
    else if(code == LV_EVENT_CLICKED) {
        file_menu_cb_t cb = (file_menu_cb_t) lv_event_get_user_data(e);
        uint32_t index = (uint32_t) lv_obj_get_user_data(obj);
        cb(index);
    }
}

static void file_menu_anim_cb(void * var, int32_t v) {
    lv_obj_t* obj = (lv_obj_t*) var;
    int32_t max_w = lv_obj_get_width(lv_obj_get_parent(obj)) - LV_DPX(20);
    int32_t w;
    w = lv_map(v, 0, 256, LV_DPX(52), max_w);
    lv_obj_set_width(obj, w);
    lv_obj_align(obj, LV_ALIGN_BOTTOM_RIGHT, - LV_DPX(10),  - LV_DPX(10));
    if(v > LV_OPA_COVER) v = LV_OPA_COVER;
    uint32_t i;
    for(i = 0; i < lv_obj_get_child_count(obj); i++) {
        lv_obj_set_style_opa(lv_obj_get_child(obj, i), v, 0);
    }
}

static void file_menu_event_cb(lv_event_t * e) {
    if(lv_event_get_code(e) == LV_EVENT_CLICKED) {
        lv_obj_t * menu_cont_w = (lv_obj_t*) lv_event_get_user_data(e);
        if(lv_obj_get_width(menu_cont_w) < LV_HOR_RES / 2) {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, menu_cont_w);
            lv_anim_set_exec_cb(&a, file_menu_anim_cb);
            lv_anim_set_values(&a, 0, 256);
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
        else {
            lv_anim_t a;
            lv_anim_init(&a);
            lv_anim_set_var(&a, menu_cont_w);
            lv_anim_set_exec_cb(&a, file_menu_anim_cb);
            lv_anim_set_values(&a, 256, 0);
            lv_anim_set_duration(&a, 200);
            lv_anim_start(&a);
        }
    }
}