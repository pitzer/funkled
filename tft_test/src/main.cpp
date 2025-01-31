#include <Arduino.h>
#include <lvgl.h>
#include "demos/lv_demos.h"
#include <TFT_eSPI.h>
#include <FT6336U.h>

// Touchscreen
#define TOUCH_RST_PIN 37
#define TOUCH_INT_PIN 30
#define TOUCH_SCL_PIN 19
#define TOUCH_SDA_PIN 18
FT6336U ft6336u(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_RST_PIN, TOUCH_INT_PIN);


// Screen resolution and rotation
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

// LVGL draws into these buffers (This is double buffered)
// Ref: https://docs.lvgl.io/8.0/porting/display.html
// "A larger buffer results in better performance but above 1/10 screen
// sized buffer(s) there is no significant performance improvement.
// Therefore it's recommended to choose the size of the draw buffer(s)
// to be at least 1/10 screen sized."
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10)
static lv_color_t draw_buf[DRAW_BUF_SIZE];

// Function declarations
static uint32_t lv_tick(void);
static void lv_print( lv_log_level_t level, const char * buf );
static void lv_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data );

//
// The main setup function
//
void setup() {
  // Serial port
  Serial.begin( 115200 );
  String lvgl_ver = "LVGL ";
  lvgl_ver += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println( lvgl_ver );

  // Touchscreen
  ft6336u.begin();

  String touch_ver = "FT6336U ";
  touch_ver += String('V') + ft6336u.read_firmware_id() + String('M') + ft6336u.read_device_mode();
  Serial.println( touch_ver );

  // Init LVGL core
  lv_init();

  // Set a tick source so that LVGL will know how much time elapsed.
  lv_tick_set_cb(lv_tick);

  // Set a print function for logging
  lv_log_register_print_cb(lv_print);

  // Register TFT_espi as the display driver
  lv_display_t * disp;
  disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
  lv_display_set_rotation(disp, TFT_ROTATION);

  // There is no native support for the FT6336U chip in LVGL, so we use
  // a dummy driver, and we will implement the touch callback ourselves.
  lv_indev_t * indev = lv_indev_create();
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
  lv_indev_set_read_cb(indev, lv_touchpad_read);

  // Change background color
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x000000), LV_PART_MAIN);

  Serial.println( "Setup done" );

  lv_demo_scroll();
}


void loop() {
  // Main LVGL loop
  lv_timer_handler();

  // Sleep a tiny bit
  delay(1);
}

// Provides LVGL with access to the timer
static uint32_t lv_tick(void)
{
  return millis();
}

// Provides LVGL with access to the serial port
static void lv_print( lv_log_level_t level, const char * buf )
{
  Serial.print("LVGL ");
  switch(level) {
    case LV_LOG_LEVEL_TRACE:
      Serial.print("TRACE: ");
      break;
    case LV_LOG_LEVEL_INFO:
      Serial.print("INFO: ");
      break;
    case LV_LOG_LEVEL_WARN:
      Serial.print("WARN: ");
      break;
    case LV_LOG_LEVEL_ERROR:
      Serial.print("ERROR: ");
      break;
    case LV_LOG_LEVEL_USER:
      Serial.print("USER: ");
      break;
    default:
      Serial.print("UNKNOWN: ");
    break;
    }
    Serial.println(buf);
    Serial.flush();
}

// Provides LVGL with access to the touchpad
// This is polled on a regular basis
static void lv_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
{
  FT6336U_TouchPointType tp = ft6336u.scan();
  data->state = tp.touch_count ? LV_INDEV_STATE_PR : LV_INDEV_STATE_REL;
  switch(TFT_ROTATION) {
    case LV_DISPLAY_ROTATION_90:
    case LV_DISPLAY_ROTATION_270:
      data->point.x = TFT_HOR_RES - 1 - tp.tp[0].x;
      data->point.y = TFT_VER_RES - 1 - tp.tp[0].y;
      break;
    case LV_DISPLAY_ROTATION_0:
    case LV_DISPLAY_ROTATION_180:
      data->point.x = tp.tp[0].x;
      data->point.y = tp.tp[0].y;
      break;
  }
}
