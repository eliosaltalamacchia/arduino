#include <Wire.h>
#include <RTClib.h>
#include <Arduino.h>
#include <RotaryEncoder.h>
#include <ezButton.h>
#include <TM1637Display.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

#define ROTARY_IN1 A2
#define ROTARY_IN2 A3
#define ROTARY_BUTTON 2
#define DISPLAY_CLK 4
#define DISPLAY_DIO 5
#define DFPLAYER_RX 10
#define DFPLAYER_TX 11

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
DateTime lastDate;


// Variables will change:
const int SHORT_PRESS_TIME = 1000; // 1 sec
int lastState = LOW;  // the previous state from the input pin
int currentState;     // the current reading from the input pin
unsigned long pressedTime  = 0;
unsigned long releasedTime = 0;
bool isConfigMode = false;

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

void detectLongPress() {
  // read the state of the switch/button:
  currentState = rotaryButton.getState();

  if(lastState == HIGH && currentState == LOW)        // button is pressed
    pressedTime = millis();
  else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();
    long pressDuration = releasedTime - pressedTime;
    if(pressDuration > SHORT_PRESS_TIME) {
      Serial.println("Entering alarm config mode");
      isConfigMode = true;
    }
  }

  // save the the last state
  lastState = currentState;
}

void displayTime(DateTime time) {
  // Create time format to display
  int displaytime = (time.hour() * 100) + time.minute();

  // Display the current time in 24 hour format with leading zeros enabled and a center colon
  display.showNumberDecEx(displaytime, 0b11100000, true);
  delay(1000);
  display.showNumberDec(displaytime, true); // Prints displaytime without center colon.
  delay(1000);
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
  softwareSerial.begin(9600);
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!dfPlayer.begin(softwareSerial)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  dfPlayer.volume(15);  //Set volume value. From 0 to 30
  dfPlayer.play(1);
}

void loop() {  
  static int pos = 0;
  encoder.tick();
  rotaryButton.loop(); 

  // get current datetime
  DateTime now = rtc.now();
  displayTime(now);
  if (now.minute() > lastDate.minute()) {
    lastDate = now;
    printDate(now);
    dfPlayer.next();
    if (dfPlayer.available()) {
      printDetail(dfPlayer.readType(), dfPlayer.read());
    }    
  }

  int newPos = encoder.getPosition();
  if (pos != newPos) {
    Serial.print("pos:");
    Serial.print(newPos);
    Serial.print(" dir:");
    Serial.println((int)(encoder.getDirection()));
    pos = newPos;
  }

  detectLongPress();
}

