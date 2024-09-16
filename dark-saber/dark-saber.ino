#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>
#include <ezButton.h>
#include <LED.h>

#define LED_STRIP_PIN 1
#define NUMPIXELS 130
#define DELAYVAL 10 // Time (in milliseconds) to pause between pixels
#define BRIGHTNESS 125 // Set BRIGHTNESS (max = 255)

#define DFPLAYER_RX 20
#define DFPLAYER_TX 21
#define DFPLAYER_VOLUME 10 //Set volume value. From 0 to 30

#define BUTTON_LED_PIN 2 // on-off switch

// Argument 1 = Number of pixels in NeoPixel strip
// Argument 2 = Arduino pin number (most are valid)
// Argument 3 = Pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip(NUMPIXELS, LED_STRIP_PIN, NEO_GRB + NEO_KHZ800);

// MPU6050
Adafruit_MPU6050 mpu;
sensors_event_t pa, pg, ptemp; // values from last reading
float movementThreshold = 0.5;

// mp3 player
SoftwareSerial softwareSerial(DFPLAYER_RX, DFPLAYER_TX); // RX, TX
DFRobotDFPlayerMini dfPlayer;

// on-off led
LED buttonLed(BUTTON_LED_PIN);

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
  dfPlayer.volume(DFPLAYER_VOLUME);  //Set volume value. From 0 to 30
}

// Print error messages from mp3 module
void printMp3ErrorDetail(uint8_t type, int value){
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

// Accelormeter & gyro initialization
void initMPU6050() {
  // Try to initialize MPU6050
  if (!mpu.begin()) {
    Serial.println("Failed to find MPU6050 chip");
    while (true) {
      delay(1000);
    }
  }
  Serial.println("MPU6050 Found!");

  //setupt motion detection
  mpu.setHighPassFilter(MPU6050_HIGHPASS_0_63_HZ);
  mpu.setMotionDetectionThreshold(10);
  mpu.setMotionDetectionDuration(1);
  mpu.setInterruptPinLatch(true);	// Keep it latched.  Will turn off when reinitialized.
  mpu.setInterruptPinPolarity(true);
  mpu.setMotionInterrupt(true);
  delay(100);
}

void printMotionDetected(sensors_event_t &a, sensors_event_t &g) {
    /* Print out the values */
    Serial.print("AccelX:");
    Serial.print(a.acceleration.x);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(a.acceleration.y);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(a.acceleration.z);
    Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(g.gyro.x);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(g.gyro.y);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(g.gyro.z);
    Serial.println("");
}

void printMotionDiff(float ax, float ay, float az, float gx, float gy, float gz) {
    /* Print out the values */
    Serial.print("AccelX:");
    Serial.print(ax);
    Serial.print(",");
    Serial.print("AccelY:");
    Serial.print(ay);
    Serial.print(",");
    Serial.print("AccelZ:");
    Serial.print(az);
    Serial.print(", ");
    Serial.print("GyroX:");
    Serial.print(gx);
    Serial.print(",");
    Serial.print("GyroY:");
    Serial.print(gy);
    Serial.print(",");
    Serial.print("GyroZ:");
    Serial.print(gz);
    Serial.println("");
}

void motionDetected() {
  sensors_event_t ca, cg, ctemp; // current values
  float diffAX, diffAY, diffAZ;
  float diffGX, diffGY, diffGZ;

  if(mpu.getMotionInterruptStatus()) {
    /* Get new sensor events with the readings */
    mpu.getEvent(&ca, &cg, &ctemp);

    // calculate movement threshold
    diffAX = ca.acceleration.x - pa.acceleration.x;
    diffAY = ca.acceleration.y - pa.acceleration.y;
    diffAZ = ca.acceleration.z - pa.acceleration.z;
    diffGX = cg.gyro.x - pg.gyro.x;
    diffGY = cg.gyro.y - pg.gyro.y;
    diffGZ = cg.gyro.z - pg.gyro.z;
    printMotionDiff(diffAX, diffAY, diffAZ, diffGX, diffGY, diffGZ);

    // choose sound to play
    int sound;
    if (diffAX >= movementThreshold) {
    }
    if (diffAY >= movementThreshold) {
    }
    if (diffAZ >= movementThreshold) {
    }
    if (diffGX >= movementThreshold) {
      sound = 3;
    }
    if (diffGY >= movementThreshold) {
      sound = 4;
    }
    if (diffGZ >= movementThreshold) {
      sound = 5;
    }

    // play selected sound
    if (dfPlayer.readState() != -1) {
      Serial.print("Playing sound: ");
      Serial.println(sound);
      dfPlayer.play(sound);
    }

    // save current values 
    pa = ca;
    pg = cg;
    ptemp = ctemp;
  }
  else {
    // play idle value
    // if (dfPlayer.readState() != -1) {
    //   dfPlayer.play(2);
    // }
  }

  delay(10);
}

void colorWipe(uint32_t color, int wait) {
  for(int i = 0, j = strip.numPixels() - 1; i < strip.numPixels(); i++, j--) { 
    strip.setPixelColor(i, color);  
    strip.setPixelColor(j, color); 
    strip.show(); 
    delay(wait);
    if (i == j) break;
  }
}

// Theater-marquee-style chasing lights. Pass in a color (32-bit value,
// a la strip.Color(r,g,b) as mentioned above), and a delay time (in ms)
// between frames.
void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  // Repeat 10 times...
    for(int b=0; b<3; b++) { //  'b' counts from 0 to 2...
      strip.clear();         //   Set all pixels in RAM to 0 (off)
      // 'c' counts up from 'b' to end of strip in steps of 3...
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); // Set pixel 'c' to value 'color'
      }
      strip.show(); // Update strip with new contents
      delay(wait);  // Pause for a moment
    }
  }
}

// Turn on leds and play sounds
// Available wav files in sd-card (must be copied in order)
// 1 -> on
// 2 -> idle
// 3 -> swing 1 - X
// 4 -> swing 2 - Y
// 5 -> swing 3 - Z
// 6 -> hit 1 - X
// 7 -> hit 2 - Y
// 8 -> hit 3 - Z
void turnOn() {
  // play on sound
  dfPlayer.play(1);
  delay(500); // short dalay to start sound & leds togheter
  
  // leds on
  strip.begin();
  strip.setBrightness(BRIGHTNESS);
  strip.clear();
  colorWipe(strip.Color(255,   255,   255), DELAYVAL); // White
  
  // play idle sound
  dfPlayer.play(2);
  delay(10);
}

void setup() {
  Serial.begin(9600);
  Serial.println("dark-saber loading ...");
  buttonLed.on();
  initMp3Player();
  turnOn();
  initMPU6050();
}

void loop() {
  motionDetected();
  // theaterChase(strip.Color(255, 255, 255), DELAYVAL); // White, full brightness
}
