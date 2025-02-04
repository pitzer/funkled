#ifndef LV_TESTER_UI_H
#define LV_TESTER_UI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "../lvgl.h"

#if LV_USE_GRID == 0
#error "LV_USE_GRID needs to be enabled"
#endif

#if LV_USE_FLEX == 0
#error "LV_USE_FLEX needs to be enabled"
#endif

// Create a complete UI for the LED tester
void led_tester_ui(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /*LV_TESTER_UI_H*/
