#include <lv_conf.h>
#include <lvgl.h>
#include "CST816S.h"
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <time.h>
#include <sntp.h>
#include <sstream>
#include <iomanip>
#include <ezLED.h>
#include "DFRobotDFPlayerMini.h"
#include <SoftwareSerial.h>
#include "ui.h"
#include "images.h"
#include "screens.h"
#include "actions.h"

// for esp32 tasks
#if CONFIG_FREERTOS_UNICORE
#define ARDUINO_RUNNING_CORE 0
#else
#define ARDUINO_RUNNING_CORE 1
#endif

// definitions for mp3 dfplayer
#if (defined(ARDUINO_AVR_UNO) || defined(ESP8266))   // Using a soft serial port
#include <SoftwareSerial.h>
SoftwareSerial softSerial(/*rx =*/4, /*tx =*/5);
#define FPSerial softSerial
#else
#define FPSerial Serial1
#endif

#define EXAMPLE_LVGL_TICK_PERIOD_MS 10
#define LED_RED 18
#define LED_BLUE 21
#define LED_RED_BLINK_ON 500
#define LED_BLUE_BLINK_ON 1000
#define LED_BLINK_OFF 250
#define LED_BLINK_INTERVAL 5000

// ntp date & time syncronization
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";
const long  gmtOffset_sec = 3600;
const int   daylightOffset_sec = 3600;
struct tm timeinfo; // current datetime info
bool isReady = false;

// available background images to use
const lv_img_dsc_t backgrounds[] = {img_star_wars_bg_1, img_star_wars_bg_2, img_star_wars_bg_3};

// mp3 player instance
DFRobotDFPlayerMini myDFPlayer;

// wi-fi configuration
const char* ssid = "***";
const char* password = "***";

// lvgl screen configuration
static const uint16_t screenWidth  = 240;
static const uint16_t screenHeight = 240;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[ screenWidth * screenHeight / 10 ];
TFT_eSPI tft = TFT_eSPI(screenWidth, screenHeight); /* TFT instance */
CST816S touch(6, 7, 13, 5);	// sda, scl, rst, irq

// head leds pins
ezLED ledBlue(LED_BLUE);
ezLED ledRed(LED_RED);

// state to indicate alarm is playing mp3 & lights
bool isPlaying = false;
bool alarmEnabled = false;

// to switch between screen states
bool isSleeping = false;

#if LV_USE_LOG
void my_log_cb(lv_log_level_t level, const char * file, uint32_t line, const char * fn_name, const char * dsc)
{
  /*Send the logs via serial port*/
  if(level == LV_LOG_LEVEL_ERROR) Serial.print("ERROR: ");
  if(level == LV_LOG_LEVEL_WARN)  Serial.print("WARNING: ");
  if(level == LV_LOG_LEVEL_INFO)  Serial.print("INFO: ");
  if(level == LV_LOG_LEVEL_TRACE) Serial.print("TRACE: ");

  char line_str[8];
  sprintf(line_str,"%d", line);
  Serial.print("#");
  Serial.println(line_str);
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
  if(count==10){
    // LV_LOG_INFO("example_increase_reboot called!: %d\n", count);
    Serial.println("example_increase_reboot called!");
    // esp_restart();
  }
}

/* Read the touchpad */
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

      // turn off leds & stop mp3
      if (isPlaying)
      {
        ledBlue.turnOFF();
        ledRed.turnOFF();
        myDFPlayer.stop();
        isPlaying = false;
      }

      // turn on screen if is sleeping
      if (isSleeping)
      {
        isSleeping = false;
        hideOff();
      }
    }
}

void hideOff()
{
  lv_obj_clear_flag(objects.seconds, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.hour, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.dots, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.minutes, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.date, LV_OBJ_FLAG_HIDDEN);
  lv_obj_clear_flag(objects.mode, LV_OBJ_FLAG_HIDDEN);
}

void hideOn()
{
  lv_obj_add_flag(objects.seconds, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.hour, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.dots, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.minutes, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.date, LV_OBJ_FLAG_HIDDEN);
  lv_obj_add_flag(objects.mode, LV_OBJ_FLAG_HIDDEN);
}

// get local datetime from ntp server and print it in the screen
void printLocalTime()
{
  struct tm timeinfo;
  static int seconds = 0;
  char buf[10];

  if(!getLocalTime(&timeinfo))
  {
    // LV_LOG_INFO("No time available (yet)\n");
    Serial.println("No time available (yet)");
    isReady = false;
  }
  else
  {
    // LV_LOG_INFO("Time Info: %A, %B %d %Y %H:%M:%S\n", &timeinfo);
    Serial.print("Time info received: ");
    sprintf(buf, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
    Serial.println(buf);
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
  // LV_LOG_INFO("Got time adjustment from NTP!\n");
  Serial.println("Got time adjustment from NTP!");
  printLocalTime();
}

// DF player mini initialization
void initMP3()
{
#if (defined ESP32)
  FPSerial.begin(9600, SERIAL_8N1, /*rx =*/16, /*tx =*/17);
#else
  FPSerial.begin(9600);
#endif

  // LV_LOG_INFO("Initializing DFPlayer ... (May take 3~5 seconds)\n");
  Serial.println("Initializing DFPlayer ... (May take 3~5 seconds)");
  if (!myDFPlayer.begin(FPSerial, /*isACK = */true, /*doReset = */true)) {  //Use serial to communicate with mp3.
    // LV_LOG_INFO("Unable to begin, please check connections & sd card\n");
    Serial.println("Unable to begin, please check connections & sd card");
    while(true){
      delay(0); // Code to compatible with ESP8266 watch dog.
    }
  }

  // LV_LOG_INFO("DFPlayer Mini online.\n");
  Serial.println("DFPlayer Mini online.");
  myDFPlayer.volume(15); 
}

void fillAlarmOptions()
{
  std::stringstream ss;
  
  // LV_LOG_INFO("Filling alarm hours & minutes\n");
  Serial.println("Filling alarm hours & minutes");
  // fill hours (0-23)
  for (int i = 0; i < 24; i++)
  {
    ss << std::setfill('0') << std::setw(2) << i << "\n";
  }
  ss.str().erase(ss.str().length() - 1); // remove last CRLF  
  lv_roller_set_options(objects.hour_alarm, ss.str().c_str(), LV_ROLLER_MODE_NORMAL);
  ss.str("");

  // fill minutes (0-59)
  for (int i = 0; i < 60; i++)
  {
    ss << std::setfill('0') << std::setw(2) << i << "\n";
  }
  ss.str().erase(ss.str().length() - 1);  
  lv_roller_set_options(objects.min_alarm, ss.str().c_str(), LV_ROLLER_MODE_NORMAL);
}

// change background images based on roller selection
void action_change_background(lv_event_t * e)
{
  lv_event_code_t code = lv_event_get_code(e);
  lv_obj_t * obj = lv_event_get_target(e);
  if(code == LV_EVENT_VALUE_CHANGED) {
    char buf[2];
    lv_roller_get_selected_str(obj, buf, sizeof(buf));
    int next = atoi(buf) - 1;
    lv_obj_set_style_bg_img_src(objects.main, &backgrounds[next], LV_PART_MAIN | LV_STATE_DEFAULT);    
    // LV_LOG_INFO("Background image changed to %d\n", next);
    Serial.print("Background image changed to ");
    Serial.println(next);
  }  
  LV_UNUSED(obj);
}

void action_blink_leds(lv_event_t * event)
{
  if (lv_obj_has_state(objects.switch_leds, LV_STATE_CHECKED))
  {
    // LV_LOG_INFO("Testing led blinking...\n");
    Serial.println("Testing led blinking...");
    ledRed.blinkInPeriod(LED_RED_BLINK_ON, LED_BLINK_OFF, LED_BLINK_INTERVAL); 
    ledBlue.blinkInPeriod(LED_BLUE_BLINK_ON, LED_BLINK_OFF, LED_BLINK_INTERVAL);
    lv_led_set_brightness(objects.led_red, 255);
    lv_led_set_brightness(objects.led_blue, 255);
  }
  else
  {
    lv_led_set_brightness(objects.led_red, 100);
    lv_led_set_brightness(objects.led_blue, 100);
  }
}

void action_change_volume(lv_event_t * event)
{
  lv_obj_t * slider = lv_event_get_target(event);
  int volume = (int)lv_slider_get_value(slider);
  myDFPlayer.volume(volume);
  delay(100);
  myDFPlayer.play(2);
  // LV_LOG_INFO("Volume changed to: %d\n", volume);
  Serial.print("Volume changed to: ");
  Serial.println(volume);
  LV_UNUSED(slider);
}

void checkAlarm()
{
  char bufHour[3];
  char bufMin[3];
  char buf[25];
  
  // check if alarm is enabled
  alarmEnabled = lv_obj_has_state(objects.alarm, LV_STATE_CHECKED);
  // LV_LOG_INFO("Checking for alarm settings - enabled = %d\n", alarmEnabled);
  Serial.print("Checking for alarm settings - enabled = ");
  Serial.println(alarmEnabled);
  if (alarmEnabled)
  {
    lv_roller_get_selected_str(objects.hour_alarm, bufHour, sizeof(bufHour));
    lv_roller_get_selected_str(objects.min_alarm, bufMin, sizeof(bufMin));
    
    // check hour & minutes agains configured alarm
    // LV_LOG_INFO("Current time: %s:%s\n", lv_label_get_text(objects.hour), lv_label_get_text(objects.minutes));
    // LV_LOG_INFO("Alarm time: %s:%s\n", bufHour, bufMin);
    sprintf(buf, "Current time: %s:%s", lv_label_get_text(objects.hour), lv_label_get_text(objects.minutes));
    Serial.println(buf);
    sprintf(buf, "Alarm time: %s:%s", bufHour, bufMin);
    Serial.println(buf);
    if (strcmp(bufHour, lv_label_get_text(objects.hour)) == 0 
      && strcmp(bufMin, lv_label_get_text(objects.minutes)) == 0)
    {
      // blink leds 
      // LV_LOG_INFO("Blinking leds...\n");
      Serial.println("Blinking leds...");
      ledBlue.blink(LED_BLUE_BLINK_ON, LED_BLINK_OFF);
      ledRed.blink(LED_RED_BLINK_ON, LED_BLINK_OFF);

      // play first mp3
      int volume = (int)lv_slider_get_value(objects.volume);
      // LV_LOG_INFO("Playing mp3 with volume = %d\n", volume);
      Serial.print("Playing mp3 with volume = ");
      Serial.println(volume);
      myDFPlayer.volume(volume);
      myDFPlayer.play(1);

      // change state to playing
      isPlaying = true;
    }
  }
}

void checkSleepMode()
{
  // check if alarm is enabled
  bool enabled = lv_obj_has_state(objects.sleep_mode, LV_STATE_CHECKED);
  Serial.print("Checking for sleep mode settings - enabled = ");
  Serial.println(enabled);
  if (enabled && !isSleeping)
  {
    // switch off screen, set hidden to false
    hideOn();
    isSleeping = true;
  }

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
      
      // update date & time every minute
      printLocalTime();

      // check alarm after update
      checkAlarm();

      // check for sleep mode
      checkSleepMode(); 
    }
    else {
      // get time every second until first update    
      if (!isReady)
      {
        printLocalTime();
      }
    }
    
    delay(call_delay);
  }
}

void setup()
{
  Serial.begin(115200); /* prepare for possible serial debug */
  Serial.println("BB8 initialization started...");
  lv_init();

  #if LV_USE_LOG
    lv_log_register_print_cb(my_log_cb);
  #endif

  String LVGL_Arduino = "Hello Arduino! ";
  LVGL_Arduino += String('V') + lv_version_major() + "." + lv_version_minor() + "." + lv_version_patch() + "\n";
  // LV_LOG_INFO(LVGL_Arduino);
  Serial.println(LVGL_Arduino);

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
   * in such a case time adjustment won't be handled automatically.
   */
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer1, ntpServer2);
  
  // change timezone
  setenv("TZ","CET-1CEST,M3.5.0,M10.5.0/3",1);
  tzset();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  // LV_LOG_INFO("Connecting to wifi");
  Serial.print("Connecting to wifi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    // LV_LOG_INFO(".");
    Serial.print(".");
  }
  // LV_LOG_INFO("\nConnected to Wi-Fi network with IP Address: ");
  // LV_LOG_INFO(WiFi.localIP());
  Serial.print("\nConnected to Wi-Fi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // create background task to get data from webservice
  uint32_t call_delay = 1000; // call blink every second 
  xTaskCreate(updateBlink, "Blink dots", 2048, (void*) &call_delay, 2, NULL);

  printLocalTime();
  initMP3();
  fillAlarmOptions();

  // LV_LOG_INFO("Setup done!\n");
  Serial.println("Setup done!\n");
}

void loop()
{
  lv_timer_handler(); /* let the GUI do its work */
  ui_tick();
  ledBlue.loop();
  ledRed.loop();
  delay(EXAMPLE_LVGL_TICK_PERIOD_MS);
}
