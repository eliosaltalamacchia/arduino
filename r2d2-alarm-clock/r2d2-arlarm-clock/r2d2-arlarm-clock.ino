#include <Wire.h>
#include <RTClib.h>
#include <RotaryEncoder.h>
#include <ezButton.h>
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>
#include <LED.h>
#include <elapsedMillis.h>

#define ROTARY_IN1 A1
#define ROTARY_IN2 A2
#define ROTARY_BUTTON 2
#define DISPLAY_CLK 5
#define DISPLAY_DIO 6
#define DISPLAY_BRIGHTNESS_MIN 1
#define DISPLAY_BRIGHTNESS_MAX 15
#define DFPLAYER_RX 10
#define DFPLAYER_TX 11
#define EEPROM_ALARM 0
#define EEPROM_ALARM_STATE 2
#define ROTARY_STEPS 1
#define ROTARY_HOUR_MIN 0
#define ROTARY_HOUR_MAX 23
#define ROTARY_MINUTE_MIN 0
#define ROTARY_MINUTE_MAX 59
#define LED_BLANK 3
#define LED_RED 8
#define LED_BLUE 9
#define LED_INTERVAL 2000
#define LED_DURATION 1000

// RTC_DS1307 rtc;
RTC_DS3231 rtc;

// 4 - 7 segments display
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(ROTARY_IN1, ROTARY_IN2, RotaryEncoder::LatchMode::FOUR3);
ezButton rotaryButton(ROTARY_BUTTON); 

// mp3 player
SoftwareSerial softwareSerial(DFPLAYER_RX, DFPLAYER_TX); // RX, TX
DFRobotDFPlayerMini dfPlayer;

// head leads
LED ledBlank(LED_BLANK);
LED ledRed(LED_RED);
LED ledBlue(LED_BLUE);

// variables saved en EPROM
int alarm = -1; // saved alarm
int alarmHr = 0; // configured alarm hour
int alarmMin = 0; // configured alarm minutes
bool isAlarmOn = false; // alarm on-off
bool isPlaying = false; 

// states for alarm configuration
enum ClockState {Clock, ShowAlarm, ConfigAlarm, ConfigOnOff};
ClockState clockState = Clock;
enum AlarmConfigState {ConfigHour, ConfigMin};
AlarmConfigState alarmConfigState = ConfigHour;

// change state in rotary
int lastPos = 0;

// alarm enabled
const uint8_t SEG_ON[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_E | SEG_G | SEG_C,  // n
  SEG_G,   
  SEG_G   
};

// alarm disabled
const uint8_t SEG_OFF[] = {
  SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
  SEG_A | SEG_F | SEG_G | SEG_E,  // F
  SEG_A | SEG_F | SEG_G | SEG_E,  // F
  SEG_G   
};

// one second info printout timer
elapsedMillis printTime;

// Print RTC time for debug
void printDate(DateTime date) {
  Serial.print(date.hour(), DEC);
  Serial.print(':');
  Serial.print(date.minute(), DEC);
  Serial.print(':');
  Serial.print(date.second(), DEC);
  Serial.println();
}

// Print error messages from mp3 module
void printDetail(uint8_t type, int value){
  switch (type) {
    case TimeOut:
      Serial.println(F("Time Out!"));
      break;
    case WrongStack:
      Serial.println(F("Stack Wrong!"));
      break;
    case DFPlayerCardInserted:
      Serial.println(F("Card Inserted!"));
      break;
    case DFPlayerCardRemoved:
      Serial.println(F("Card Removed!"));
      break;
    case DFPlayerCardOnline:
      Serial.println(F("Card Online!"));
      break;
    case DFPlayerPlayFinished:
      Serial.print(F("Number:"));
      Serial.print(value);
      Serial.println(F(" Play Finished!"));
      break;
    case DFPlayerError:
      Serial.print(F("DFPlayerError:"));
      switch (value) {
        case Busy:
          Serial.println(F("Card not found"));
          break;
        case Sleeping:
          Serial.println(F("Sleeping"));
          break;
        case SerialWrongStack:
          Serial.println(F("Get Wrong Stack"));
          break;
        case CheckSumNotMatch:
          Serial.println(F("Check Sum Not Match"));
          break;
        case FileIndexOut:
          Serial.println(F("File Index Out of Bound"));
          break;
        case FileMismatch:
          Serial.println(F("Cannot Find File"));
          break;
        case Advertise:
          Serial.println(F("In Advertise"));
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

// Clock mode show current time
// Dots blinking every second
void displayTime(DateTime time) {
  static bool isDotsOn = false;

  // Create time format to display
  int displaytime = (time.hour() * 100) + time.minute();
  if (printTime >= 1000) {
    printTime = 0;
    isDotsOn = !isDotsOn;
  }

  if (isDotsOn) {
    // Display the current time in 24 hour format with leading zeros enabled and a center colon
    display.showNumberDecEx(displaytime, 0b01000000, true);
  }
  else {
    display.showNumberDec(displaytime, true); // Prints displaytime without center colon.
  }
}

// Show configured alarm blinking
void displayAlarm() {
  static bool isDotsOn = false;

  if (printTime >= 500) {
    printTime = 0;
    isDotsOn = !isDotsOn;
  }

  // show configured alarm blinking
  if (isDotsOn) {
    // Display the current time in 24 hour format with leading zeros enabled and a center colon
    display.showNumberDecEx(alarm, 0b01000000, true);
  }
  else {
    display.clear();
  }
}

// For config mode show first 2 digit hour 
// After click show two digits minutes
void displayTwoDigitsAlarm(int twoDigitsTime, bool leftmost) {
  static bool isDotsOn = false;

  if (printTime >= 500) {
    printTime = 0;
    isDotsOn = !isDotsOn;
  }

  if (isDotsOn) {
    // Display the current time in 24 hour format with leading zeros enabled and a center colon
    display.showNumberDecEx(twoDigitsTime, 0b11100000, true, 2, (leftmost ? 0 : 2));
  }
  else {
    display.clear();
  }
}

// Read alarm from arduino's EPROM
// Read position 0 -> int alarm
// Read position 2 -> on/off
int readAlarm() {
  EEPROM.get(EEPROM_ALARM, alarm);
  EEPROM.get(EEPROM_ALARM_STATE, isAlarmOn);
  Serial.print("Reading saved alarm: ");
  Serial.print(alarm);
  Serial.print("/");
  Serial.println(isAlarmOn);
  Serial.print("Setting alarm to: ");
  if (alarm < 0) {
    // default to current time
    DateTime time = rtc.now();
    alarm = (time.hour() * 100) + time.minute();
    Serial.print(" default current time ");
  }
  Serial.println(alarm);

  return alarm;
}

// Save alarm to arduino's EPROM
// Save position 0 -> int alarm
// Save position 2 -> on/off
void saveAlarm() {
  alarm = alarmHr * 100 + alarmMin; 
  Serial.print("Saving new alarm: ");
  Serial.print(alarm);
  Serial.print("/");
  Serial.println(isAlarmOn);
  EEPROM.put(EEPROM_ALARM, alarm);
  EEPROM.put(EEPROM_ALARM_STATE, isAlarmOn);
}

// Read value from rotary encoder
int getNewValue(int minValue, int maxValue, int steps) {
  int newPos = encoder.getPosition();
  if (newPos < minValue) {
    encoder.setPosition(minValue / steps);
    newPos = minValue;
  } 
  else if (newPos > maxValue) {
    encoder.setPosition(maxValue / steps);
    newPos = maxValue;
  }

  if (lastPos != newPos) {
    Serial.println(newPos);
    lastPos = newPos;
  }

  return newPos;
}

// Read value from rotary encoder
void showAlarmEnabledDisabled() {
  int newPos = encoder.getPosition();
  if (lastPos != newPos) {
    if (isAlarmOn) {
      isAlarmOn = false;
    }
    else {
      isAlarmOn = true;
    }
    lastPos = newPos;
    Serial.print("Change alarm state: ");
    Serial.println(isAlarmOn ? "ON" : "OFF");
  }

  if (isAlarmOn) {
    display.setSegments(SEG_ON);  
  }
  else {
    display.setSegments(SEG_OFF);  
  }
}

// Alarm configuration
// 1 click -> show blinking configured alarm
// 1 click -> on/off
// 1 click -> if off then goto clock mode
// 1 click -> if on then configure hour
// 1 click -> configure minutes
// 1 click -> return to clock
void alarmConfig() {
  int newValue = 0;

  if (alarmConfigState == ConfigHour) {
    // show configured alarm (blinking) separating hour and minutes
    newValue = getNewValue(ROTARY_HOUR_MIN, ROTARY_HOUR_MAX, ROTARY_STEPS);
    displayTwoDigitsAlarm(newValue, true);
    if (rotaryButton.isPressed()) {
      Serial.println("Change to minutes configuration");
      alarmHr = newValue;
      encoder.setPosition(alarmMin);
      alarmConfigState = ConfigMin;
    }
  }
  else if (alarmConfigState == ConfigMin) {
    // show minutes blinking
    newValue = getNewValue(ROTARY_MINUTE_MIN, ROTARY_MINUTE_MAX, ROTARY_STEPS);
    displayTwoDigitsAlarm(newValue, false);
    if (rotaryButton.isPressed()) {
      Serial.println("Exit alarm config mode");
      alarmMin = newValue;
      alarmConfigState = ConfigHour;
      clockState = Clock;
      saveAlarm();
    }
  }
}

// Play sound based on configured alarm time
void playAlarm(DateTime now) {
  static bool waitNext = false;

  if (isAlarmOn && !isPlaying) {
    int time = (now.hour() * 100) + now.minute();
    if (time == alarm) {
      if (!waitNext) {
        // play mp3
        Serial.println("Playing alarm sound");
        dfPlayer.next();
        if (dfPlayer.available()) {
          printDetail(dfPlayer.readType(), dfPlayer.read());
        }
        display.setBrightness(DISPLAY_BRIGHTNESS_MAX);         
        isPlaying = true; // wait until button press to stop
        waitNext = true; // play cycle once during the same minute
      }    
    }
    else {
      waitNext = false;
    }
  }
}

// DFPlayer mp3 module initialization
void initMp3Player() {
  softwareSerial.begin(9600);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!dfPlayer.begin(softwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  dfPlayer.volume(20);  //Set volume value. From 0 to 30
  dfPlayer.enableLoopAll();
}

void setup() {
  Serial.begin(115200);

  // check for RTC
  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (true);
  }

  // set date/time
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // if (rtc.lostPower()) {
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // }

  // debounce rotary button
  rotaryButton.setDebounceTime(30);

  // Set the display brightness (0-7):
  display.setBrightness(DISPLAY_BRIGHTNESS_MIN);
  display.clear();

  // mp3 player for alarm sounds
  initMp3Player();

  // led flash interval
  ledBlank.begin(LED_INTERVAL);
  ledRed.begin(LED_INTERVAL);
  ledBlue.begin(LED_INTERVAL);
  
  // Read configured alarm from EPROM
  alarm = readAlarm();
  alarmHr = alarm / 100;
  alarmMin = alarm - (alarmHr * 100);
}

void loop() {

  // poll for changes
  rotaryButton.loop(); 
  encoder.tick();
  
  // show time until config
  if (clockState == Clock) {
    DateTime now = rtc.now();
    displayTime(now); // show current time from RTC
    playAlarm(now); // check alarm and play mp3

    if (isPlaying) {
      ledBlank.flash(0, LED_DURATION);
      ledRed.flash(0, LED_DURATION);
      ledBlue.on();
    }

    if (rotaryButton.isPressed()) {
      if (isPlaying) {
        // is playing alarm then stop playing mp3
        Serial.println("Stopping alarm sounds");
        dfPlayer.stop();
        ledBlank.off();
        ledRed.off();
        ledBlue.off();
        display.setBrightness(DISPLAY_BRIGHTNESS_MIN);
        isPlaying = false;
      }
      else {
        Serial.println("Showing alarm");
        clockState = ShowAlarm;
        lastPos = 0;
        encoder.setPosition(lastPos);
      }
    }
  }
  else if (clockState == ShowAlarm) {
    // show configured alarm blinking
    displayAlarm();
    if (rotaryButton.isPressed()) {
      Serial.println("Enable/disable alarm");
      clockState = ConfigOnOff;
    }
  }
  else if (clockState == ConfigOnOff) {
    // show on/off
    showAlarmEnabledDisabled();
    if (rotaryButton.isPressed()) {
      if (isAlarmOn) {
        Serial.println("Enter alarm config mode");
        encoder.setPosition(alarmHr);
        clockState = ConfigAlarm;
      }
      else {
        Serial.println("Alarm disabled, return to clock mode");
        saveAlarm();
        clockState = Clock;
      }
    }
  }
  else if (clockState == ConfigAlarm) {
    // change alarm configuration
    alarmConfig();
  }
}
