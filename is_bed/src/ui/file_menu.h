#ifndef FILE_MENU_H
#define FILE_MENU_H

#include <lvgl.h>

// Callback type for when a menu item is clicked
typedef void (*file_menu_cb_t)();

// Menu item descriptor
typedef struct {
    const char* message;
    const char* icon;
    file_menu_cb_t cb;
} file_menu_item_t;

// Create a file menu like item on the bottom right of the screen
lv_obj_t* file_menu_create(lv_obj_t* parent, const file_menu_item_t* items, uint32_t num_items);

#endif // FILE_MENU_H