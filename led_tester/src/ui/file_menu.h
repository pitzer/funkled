#ifndef FILE_MENU_H
#define FILE_MENU_H

#include <lvgl.h>

// Callback type for when a menu item is clicked
typedef void (*file_menu_cb_t)(uint32_t index);

// Create a file menu like item on the bottom right of the screen
lv_obj_t* file_menu_create(lv_obj_t* parent, const char** items, file_menu_cb_t cb);

#endif // FILE_MENU_H