#include <lvgl.h>
#include <TFT_eSPI.h>
#include "lv_conf.h"
#include "CST816S.h"
#include "ui.h"
#include <WiFi.h>
#include <time.h>
#include <sntp.h>
#include <sstream>
#include <iomanip>
#include "images.h"

#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

#define EXAMPLE_LVGL_TICK_PERIOD_MS    2

const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
const char* time_zone = "CET-1CEST,M3.5.0,M10.5.0/3";  // TimeZone rule for Europe/Rome including daylight adjustment rules (optional)
struct tm timeinfo; 
bool isReady = false;

const lv_img_dsc_t backgrounds[] = {img_star_wars_bg_1, img_star_wars_bg_2, img_star_wars_bg_3};

TaskHandle_t analog_read_task_handle; // You can (don't have to) use this to be able to manipulate a task from somewhere else.

// Replace with your network credentials
const char* ssid = "elios_Guest";
const char* password = "#elios2024@";
// const char* ssid = "Sh";
// const char* password = "p26cYXkJ4nKP";

/*Change to your screen resolution*/
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;

static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];

TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
CST816S touch(6, 7, 13, 5);	// sda, scl, rst, irq

#if LV_USE_LOG != 0
/* Serial debugging */
void my_print(const char * buf)
{
    Serial.printf(buf);
    Serial.flush();
}
#endif

/* Display flushing */
void my_disp_flush( lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p )
{
    uint32_t w = ( area->x2 - area->x1 + 1 );
    uint32_t h = ( area->y2 - area->y1 + 1 );

    tft.startWrite();
    tft.setAddrWindow( area->x1, area->y1, w, h );
    tft.pushColors( ( uint16_t * )&color_p->full, w * h, true );
    tft.endWrite();

    lv_disp_flush_ready( disp_drv );
}

void example_increase_lvgl_tick(void *arg)
{
    /* Tell LVGL how many milliseconds has elapsed */
    lv_tick_inc(EXAMPLE_LVGL_TICK_PERIOD_MS);
}

static uint8_t count=0;
void example_increase_reboot(void *arg)
{
  count++;
  if(count==30){
    // esp_restart();
  }
}

/*Read the touchpad*/
void my_touchpad_read( lv_indev_drv_t * indev_drv, lv_indev_data_t * data )
{
    static int next = 1; // start with 1 for the first change

    // uint16_t touchX, touchY;
    bool touched = touch.available();
    // touch.read_touch();
    if( !touched )
    // if( 0!=touch.data.points )
    {
        data->state = LV_INDEV_STATE_REL;
    }
    else
    {
      data->state = LV_INDEV_STATE_PR;

      /*Set the coordinates*/
      data->point.x = touch.data.x;
      data->point.y = touch.data.y;
      Serial.print("(");
      Serial.print(touch.data.x);
      Serial.print(", ");
      Serial.print(touch.data.y);
      Serial.println(")");

      lv_img_set_src(objects.background, &backgrounds[next++]);
      if (next == (sizeof(backgrounds) / sizeof(lv_img_dsc_t))) next = 0;
    }
}

void printLocalTime()
{
  struct tm timeinfo;
  static int seconds = 0;

  if(!getLocalTime(&timeinfo))
  {
    Serial.println("No time available (yet)");
    isReady = false;
  }
  else
  {
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(2) << timeinfo.tm_hour;
    lv_label_set_text(objects.hour, ss.str().c_str());
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << timeinfo.tm_min;
    lv_label_set_text(objects.minutes, ss.str().c_str());
    ss.str("");
    ss << std::setfill('0') << std::setw(2) << timeinfo.tm_mday << '/' 
      << std::setfill('0') << std::setw(2) << timeinfo.tm_mon + 1 << '/' 
      << timeinfo.tm_year % 100;  
    lv_label_set_text(objects.date, ss.str().c_str());
    isReady = true;
  }
}

// Callback function (get's called when time adjusts via NTP)
void timeavailable(struct timeval *t)
{
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

void updateBlink(void *pvParameters)
{
  static int seconds = 0;
  static bool isOn = true; // isOn is used to blink clock dots
  uint32_t call_delay = *((uint32_t*)pvParameters);

  for (;;)
  {
    // blink dots & update seconds bar
    lv_arc_set_value(objects.seconds, seconds++);
    lv_label_set_text(objects.dots, isOn ? ":" : "");
    isOn = !isOn;
    
    if(seconds == 60)
    {
      seconds = 0;
      printLocalTime(); // update every minute
    }

    // get time every second until first update    
    if (!isReady)
    {
      printLocalTime();
    }
    
    delay(call_delay);
  }
}

void action_change_background(lv_event_t * event)
{
  static int bg = 1;
  Serial.println(bg);
  switch (bg) {
    case 1:
      lv_img_set_src(objects.background, &img_star_wars_bg_1);
      break;
    case 2:
      lv_img_set_src(objects.background, &img_star_wars_bg_2);
      break;
    case 3:
      lv_img_set_src(objects.background, &img_star_wars_bg_3);
      bg = 0;
      break;
  }
  bg++;
}

void action_night_mode(lv_event_t * event)
{
  Serial.println("action_night_mode");
}

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch();
  Serial.println( LVGL_Arduino );
  Serial.println( "I am LVGL_Arduino" );

  lv_init();
#if LV_USE_LOG != 0
  lv_log_register_print_cb( my_print ); /* register print function for debugging */
#endif

  tft.begin(); /* TFT init */
  tft.setRotation(0); /* Landscape orientation, flipped */
  
  /*Set the touchscreen calibration data,
    the actual data for your display can be acquired using
    the Generic -> Touch_calibrate example from the TFT_eSPI library*/
  // uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
  // tft.setTouch( calData );
  touch.begin();

  lv_disp_draw_buf_init( &draw_buf, buf, NULL, screenWidth * screenHeight / 10 );

  /*Initialize the display*/
  static lv_disp_drv_t disp_drv;
  lv_disp_drv_init( &disp_drv );

  /*Change the following line to your display resolution*/
  disp_drv.hor_res = screenWidth;
  disp_drv.ver_res = screenHeight;
  disp_drv.flush_cb = my_disp_flush;
  disp_drv.draw_buf = &draw_buf;
  lv_disp_drv_register( &disp_drv );

  /*Initialize the (dummy) input device driver*/
  static lv_indev_drv_t indev_drv;
  lv_indev_drv_init( &indev_drv );
  indev_drv.type = LV_INDEV_TYPE_POINTER;
  indev_drv.read_cb = my_touchpad_read;
  lv_indev_drv_register( &indev_drv );

  const esp_timer_create_args_t lvgl_tick_timer_args = {
    .callback = &example_increase_lvgl_tick,
    .name = "lvgl_tick"
  };

  const esp_timer_create_args_t reboot_timer_args = {
    .callback = &example_increase_reboot,
    .name = "reboot"
  };

  esp_timer_handle_t lvgl_tick_timer = NULL;
  esp_timer_create(&lvgl_tick_timer_args, &lvgl_tick_timer);
  esp_timer_start_periodic(lvgl_tick_timer, EXAMPLE_LVGL_TICK_PERIOD_MS * 1000);

  esp_timer_handle_t reboot_timer = NULL;
  esp_timer_create(&reboot_timer_args, &reboot_timer);
  esp_timer_start_periodic(reboot_timer, 2000 * 1000);
  
  ui_init();

  // set notification call-back function
  sntp_set_time_sync_notification_cb(timeavailable);

  /**
   * NTP server address could be aquired via DHCP,
   *
   * NOTE: This call should be made BEFORE esp32 aquires IP address via DHCP,
   * otherwise SNTP option 42 would be rejected by default.
   * NOTE: configTime() function call if made AFTER DHCP-client run
   * will OVERRIDE aquired NTP server address
   */
  sntp_servermode_dhcp(1);    // (optional)

  /**
   * This will set configured ntp servers and constant TimeZone/daylightOffset
   * should be OK if your time zone does not need to adjust daylightOffset twice a year,
   * in such a case time adjustment won't be handled automagicaly.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to wifi ");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.print("\nConnected to Wi-Fi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // create background task to get data from webservice
  uint32_t call_delay = 1000; // call blink every second 
  xTaskCreate(updateBlink, "Blink dots", 2048, (void*) &call_delay, 2, NULL);

  printLocalTime();

  Serial.println( "Setup done" );
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  delay(1000);
}
