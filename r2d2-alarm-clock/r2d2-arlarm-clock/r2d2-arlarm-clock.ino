#include <Wire.h>
#include <RTClib.h>
#include <RotaryEncoder.h>
#include <ezButton.h>
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <EEPROM.h>
#include <elapsedMillis.h>

#define ROTARY_IN1 A2
#define ROTARY_IN2 A3
#define ROTARY_BUTTON 2
#define DISPLAY_CLK 4
#define DISPLAY_DIO 5
#define DFPLAYER_RX 10
#define DFPLAYER_TX 11
#define EEPROM_ALARM 0
#define EEPROM_ALARM_STATE 2
#define ROTARY_STEPS 1
#define ROTARY_HOUR_MIN 0
#define ROTARY_HOUR_MAX 23
#define ROTARY_MINUTE_MIN 0
#define ROTARY_MINUTE_MAX 59

// RTC_DS1307 rtc;
RTC_DS3231 rtc;

// 4 - 7 segments display
TM1637Display display(DISPLAY_CLK, DISPLAY_DIO);
uint8_t data[] = { 0xff, 0xff, 0xff, 0xff };
uint8_t blank[] = { 0x00, 0x00, 0x00, 0x00 };

// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
RotaryEncoder encoder(ROTARY_IN1, ROTARY_IN2, RotaryEncoder::LatchMode::TWO03);
ezButton rotaryButton(ROTARY_BUTTON); 

// mp3 player
SoftwareSerial softwareSerial(DFPLAYER_RX, DFPLAYER_TX); // RX, TX
DFRobotDFPlayerMini dfPlayer;

// clock helpers
String daysOfTheWeek[7] = { "Domingo", "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado" };
String monthsNames[12] = { "Enero", "Febrero", "Marzo", "Abril", "Mayo",  "Junio", "Julio","Agosto","Septiembre","Octubre","Noviembre","Diciembre" };

// To detect long press for config mode
const int SHORT_PRESS_TIME = 1500; 
int lastState = HIGH;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isConfigMode = false;
bool isConfigOff = false;
int alarmHr = 0;
int alarmMin = 0;
byte alarmOff = 0;
enum AlarmState {Alarm, Hour, Minute};

// one second info printout timer
elapsedMillis printTime;

void printDate(DateTime date) {
  Serial.print(date.year(), DEC);
  Serial.print('/');
  Serial.print(date.month(), DEC);
  Serial.print('/');
  Serial.print(date.day(), DEC);
  Serial.print(" (");
  Serial.print(daysOfTheWeek[date.dayOfTheWeek()]);
  Serial.print(") ");
  Serial.print(date.hour(), DEC);
  Serial.print(':');
  Serial.print(date.minute(), DEC);
  Serial.print(':');
  Serial.print(date.second(), DEC);
  Serial.println();
}

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

void displayAlarm(int alarm) {
  static bool isDotsOn = false;

  if (printTime >= 500) {
    printTime = 0;
    isDotsOn = !isDotsOn;
  }

  if (isDotsOn) {
    // Display the current time in 24 hour format with leading zeros enabled and a center colon
    display.showNumberDecEx(alarm, 0b01000000, true);
  }
  else {
    display.clear();
  }
}

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

void displayAlarmOff() {
  // alarm disabled
  const uint8_t SEG_OFF[] = {
    SEG_A | SEG_B | SEG_C | SEG_D | SEG_E | SEG_F,   // O
    SEG_A | SEG_F | SEG_G | SEG_E,  // F
    SEG_A | SEG_F | SEG_G | SEG_E,  // F
    SEG_G   
    };

  display.setSegments(SEG_OFF);
  if (rotaryButton.isPressed()) {
    Serial.println("Alarm disabled");
    saveAlarm(-1); // disabled
  }
}

int readAlarm() {
  int alarm = 0;
  
  EEPROM.get(EEPROM_ALARM, alarm);
  EEPROM.get(EEPROM_ALARM_STATE, alarmOff);
  Serial.print("Reading saved alarm: ");
  Serial.print(alarm);
  Serial.print("/");
  Serial.println(alarmOff);
  if (alarm < 0) {
    // default to current time
    DateTime time = rtc.now();
    alarm = (time.hour() * 100) + time.minute();
  }
  Serial.print("Setting default alarm to: ");
  Serial.println(alarm);

  return alarm;
}

void saveAlarm(int alarm) {
  byte off = isConfigOff ? 1 : 0;
  Serial.print("Saving new alarm: ");
  Serial.print(alarm);
  Serial.print("/");
  Serial.println(off);
  EEPROM.put(EEPROM_ALARM, alarm);
  EEPROM.put(EEPROM_ALARM_STATE, off);
}

int getNewValue(int minValue, int maxValue, int steps) {
  static int lastPos = 0;

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
    // Serial.println(newPos);
    lastPos = newPos;
  }

  return newPos;
}

bool detectLongPress() {
  bool isLongPress = false;
  static int clickCount = 0;

  // read the state of the switch/button:
  currentState = rotaryButton.getState();

  if(lastState == HIGH && currentState == LOW) {        // button is pressed
    pressedTime = millis();
  }
  else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    if(pressDuration > SHORT_PRESS_TIME) {
      Serial.println("Long press detected");
      isLongPress = true;
      isConfigOff = true;
    }
    else {
      Serial.println("Short press detected");
      isLongPress = false;
      clickCount++;
      if (clickCount == 1) {
        if (isConfigOff) {
          isConfigOff = false;
        }
        else {
          isConfigMode = true;
        }
      }
      else if (clickCount == 4) {
        clickCount = 0;
        isConfigMode = false;
      }
    }
  }

  // save the the last state
  lastState = currentState;
  return isLongPress;
}

// Alarm configuration
// 1 click -> show blinking configured alarm
// 1 click -> configure hour
// 1 click -> configure minutes
// 1 click -> return to clock
// 1 long click -> switch from alarm on/off
void alarmConfig() {
  static AlarmState state = Alarm;
  int newValue = 0;

  if (state == Alarm) {
    int alarm = alarmHr * 100 + alarmMin;
    displayAlarm(alarm);
    if (rotaryButton.isPressed()) {
      Serial.println("Changing to hour configuration");
      encoder.setPosition(alarmHr);
      state = Hour;
    }
  }
  else if (state == Hour) {
    // show configured alarm (blinking) separating hour and minutes
    newValue = getNewValue(ROTARY_HOUR_MIN, ROTARY_HOUR_MAX, ROTARY_STEPS);
    displayTwoDigitsAlarm(newValue, true);
    if (rotaryButton.isPressed()) {
      Serial.println("Change to minutes configuration");
      alarmHr = newValue;
      encoder.setPosition(alarmMin);
      state = Minute;
    }   
  }
  else if (state == Minute) {
    // show minutes blinking
    newValue = getNewValue(ROTARY_MINUTE_MIN, ROTARY_MINUTE_MAX, ROTARY_STEPS);
    displayTwoDigitsAlarm(newValue, false);
    if (rotaryButton.isPressed()) {
      Serial.println("Exit alarm config mode");
      alarmMin = newValue;
      int alarm = alarmHr * 100 + alarmMin;
      saveAlarm(alarm);
      state = Alarm;
    }   
  }
}

void playAlarm(DateTime now) {
  int alarm = readAlarm();
  int time = (now.hour() * 100) + now.minute();
  if (time == alarm) {
    Serial.println("Playing alarm sound");
    dfPlayer.next();
    if (dfPlayer.available()) {
      printDetail(dfPlayer.readType(), dfPlayer.read());
    } 
  }
}

void setup() {
  Serial.begin(115200);

  // check for RTC
  if (!rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
    while (true);
  }

  // set date/time
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // debounce rotary button
  rotaryButton.setDebounceTime(50);

  // Set the display brightness (0-7):
  display.setBrightness(1);
  display.clear();

  // DFPlayer mp3 module initialization
  // softwareSerial.begin(9600);
  // Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  // if (!dfPlayer.begin(softwareSerial)) {  //Use softwareSerial to communicate with mp3.
  //   Serial.println(F("Unable to begin:"));
  //   Serial.println(F("1.Please recheck the connection!"));
  //   Serial.println(F("2.Please insert the SD card!"));
  //   // while(true);
  // }
  // Serial.println(F("DFPlayer Mini online."));
  // dfPlayer.volume(15);  //Set volume value. From 0 to 30

  // Read configured alarm
  int alarm = readAlarm();
  alarmHr = alarm / 100;
  alarmMin = alarm - (alarmHr * 100);
}

void loop() {  
  encoder.tick();
  rotaryButton.loop(); 

  bool isLongPress = detectLongPress(); 
  if (isLongPress || isConfigOff) {
    // turn off alarm
    displayAlarmOff();
  }
  else {
    if (isConfigMode) {
      // show configured alarm
      alarmConfig();
    }
    else {
      // show current time from RTC
      DateTime now = rtc.now();
      displayTime(now);
    }
  }
}
