#include "led_array.h"
#include <Arduino.h>
#include <lvgl.h>
#include "ui/led_tester_ui.h"
#include <TFT_eSPI.h>
#include <FT6336U.h>
#include <OctoWS2811.h>


// Touchscreen
#define TOUCH_RST_PIN 37
#define TOUCH_INT_PIN 30
#define TOUCH_SCL_PIN 19
#define TOUCH_SDA_PIN 18
FT6336U ft6336u(TOUCH_SDA_PIN, TOUCH_SCL_PIN, TOUCH_RST_PIN, TOUCH_INT_PIN);

// Backlight
#define BACKLIGHT_PIN 29

// Status LED
#define STATUS_RED 33
#define STATUS_GREEN 32

// LEDs
#define LED_REFRESH_RATE_HZ 20
const uint8_t pin_list [] = { 28, 24, 15, 7, 5, 3, 2, 1, 25, 14, 8, 6, 4, 22, 23,  0};
const uint32_t bytes_per_led = 3;
DMAMEM uint8_t display_memory[max_leds * bytes_per_led];
uint8_t drawing_memory[max_leds * bytes_per_led];
// OctoWS2811 can do its own RGB reordering, but it may be different for each strip, so we do it ourselves.
const uint8_t config = WS2811_RGB | WS2811_800kHz;
OctoWS2811 leds(max_leds_per_channel, display_memory, drawing_memory, config, num_led_channels, pin_list);


// Screen resolution and rotation
#ifdef HOSYOND_3_2_TFT
  #define TFT_HOR_RES   240
  #define TFT_VER_RES   320
#endif
#ifdef HOSYOND_4_0_TFT
  #define TFT_HOR_RES   320
  #define TFT_VER_RES   480
#endif
#define TFT_ROTATION  LV_DISPLAY_ROTATION_90

// LVGL draws into these buffers (This is double buffered)
// Ref: https://docs.lvgl.io/8.0/porting/display.html
// "A larger buffer results in better performance but above 1/10 screen
// sized buffer(s) there is no significant performance improvement.
// Therefore it's recommended to choose the size of the draw buffer(s)
// to be at least 1/10 screen sized."
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10)
static lv_color_t draw_buf[DRAW_BUF_SIZE];

// Function prototypes
static uint32_t lv_tick(void);
static void lv_print( lv_log_level_t level, const char * buf );
static void lv_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data );
static void led_refresh_cb(lv_timer_t * timer);

//
// The main setup function
//
void setup() {
  // Status LED
  pinMode(STATUS_RED, OUTPUT);
  pinMode(STATUS_GREEN, OUTPUT);
  digitalWrite(STATUS_RED, HIGH);
  digitalWrite(STATUS_GREEN, HIGH);

  // Serial port
  Serial.begin( 115200 );
  String lvgl_ver = "LVGL ";
  lvgl_ver += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println( lvgl_ver );

  // Touchscreen
  ft6336u.begin();
  // Set the interrupt to polling mode (low when there is a touch, high otherwise)
  ft6336u.write_g_mode(pollingMode);
  String touch_ver = "FT6336U ";
  touch_ver += String('V') + ft6336u.read_firmware_id() + String('M') + ft6336u.read_device_mode();
  Serial.println( touch_ver );

  // Backlight
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);

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

  // Initialize the led array descriptors
  led_array_init();

  // Build the UI
  led_tester_ui();

  // Start the LEDs
  leds.begin();

  // Register the refresh function. We are going to use an LVGL timer to call this function.
  lv_timer_create(led_refresh_cb, 1000 / LED_REFRESH_RATE_HZ, NULL);

  // We are done
  Serial.println( "Setup done" );
  digitalWrite(STATUS_GREEN, HIGH);
  digitalWrite(STATUS_RED, LOW);

}


void loop() {
  // Main LVGL loop
  lv_timer_handler();
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
  if (digitalRead(TOUCH_INT_PIN) == HIGH) {
    // Nothing new. Return early
    return;
  }

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

// Refresh the LEDs
static void led_refresh_cb(lv_timer_t * timer)
{
  uint32_t now = millis();
  for (uint32_t i = 0; i < num_led_channels; i++) {
    // Temporary buffer to render the LEDs
    CRGB leds_crgb[led_strings[i].num_leds];
    led_patterns[led_strings[i].pattern_index].update(
      now,
      led_strings[i].update_period_ms,
      composed_palette(&led_palettes[led_strings[i].palette_index], led_strings[i].single_color),
      led_strings[i].single_color,
      led_strings[i].num_leds,
      leds_crgb);
    for (uint32_t j = 0; j < max_leds_per_channel; j++) {
      uint32_t color_u32 = 0x000000;
      if (j < led_strings[i].num_leds) {
        leds_crgb[j].nscale8(led_strings[i].brightness);
        switch(led_strings[i].color_ordering) {
          case WS2811_RGB:
            color_u32 = leds_crgb[j].r << 16 | leds_crgb[j].g << 8 | leds_crgb[j].b;
            break;
          case WS2811_RBG:
            color_u32 = leds_crgb[j].r << 16 | leds_crgb[j].b << 8 | leds_crgb[j].g;
            break;
          case WS2811_GRB:
            color_u32 = leds_crgb[j].g << 16 | leds_crgb[j].r << 8 | leds_crgb[j].b;
            break;
          case WS2811_GBR:
            color_u32 = leds_crgb[j].g << 16 | leds_crgb[j].b << 8 | leds_crgb[j].r;
            break;
          case WS2811_BRG:
            color_u32 = leds_crgb[j].b << 16 | leds_crgb[j].r << 8 | leds_crgb[j].g;
            break;
          case WS2811_BGR:
            color_u32 = leds_crgb[j].b << 16 | leds_crgb[j].g << 8 | leds_crgb[j].r;
            break;
          default:
            color_u32 = 0x000000;
            break;
        }
      }
      leds.setPixel(i * max_leds_per_channel + j, color_u32);
    }
  }
  leds.show();
}