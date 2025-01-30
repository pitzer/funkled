#include <Arduino.h>
#include <lvgl.h>
#include <TFT_eSPI.h>

// Screen resolution and rotation
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_DISPLAY_ROTATION_90

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


static lv_obj_t *label;
static int pos = 0;
static int dir = 1;


//
// The main setup function
//
void setup() {
  // Serial port
  Serial.begin( 115200 );
  String LVGL_Ver = "LVGL ";
  LVGL_Ver += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println( LVGL_Ver );

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
  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xffffff), LV_PART_MAIN);

  // Create a label
  label = lv_label_create( lv_screen_active() );
  lv_label_set_text( label, "FunkLED" );
  // increase font size
  lv_obj_set_style_text_font(label, &lv_font_montserrat_44,  LV_PART_MAIN);
  lv_obj_align( label, LV_ALIGN_CENTER, 0, 0 );

  Serial.println( "Setup done" );
}


void loop() {

  lv_obj_align( label, LV_ALIGN_CENTER, pos / 5, 0 );
  if (dir) {
    pos += 1;
  } else {
    pos -= 1;
  }
  if (pos > 150) {
    dir = 0;
  }
  if (pos < -150) {
    dir = 1;
  }


  // Main LVGL loop
  lv_timer_handler();

  // Wait for a little bit
  delay(5);
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
    /*For example  ("my_..." functions needs to be implemented by you)
    int32_t x, y;
    bool touched = my_get_touch( &x, &y );

    if(!touched) {
        data->state = LV_INDEV_STATE_RELEASED;
    } else {
        data->state = LV_INDEV_STATE_PRESSED;

        data->point.x = x;
        data->point.y = y;
    }
     */
}
