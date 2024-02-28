#include <FastLED.h>
 
#define LED_PIN     4
#define NUM_LEDS    10
#define BRIGHTNESS  64
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB 
#define BUTTON_PIN  3
#define UPDATES_PER_SECOND 100
#define FPS 50
#define FLASHES 8
#define FREQUENCY 2 // delay between strikes

CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType currentBlending;
 
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;

CRGB::HTMLColorCode colorOptions[] = {CRGB::Blue, CRGB::Red, CRGB::Green, CRGB::White};
unsigned int selected = 0;
unsigned int dimmer = 1;
int flashCounter = 0;
int cycle = random8(3, FLASHES);

// Variables will change:
int ledState = HIGH;        // the current state of the output pin
int buttonState;            // the current reading from the input pin
int lastButtonState = LOW;  // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup() { 
  Serial.begin(9600);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  delay( 3000 ); // power-up safety delay
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
  currentPalette = CloudColors_p;
  currentBlending = LINEARBLEND;
  Serial.println("Starting...");
}
 
void loop() { 
  // read the state of the switch into a local variable:
  int reading = digitalRead(BUTTON_PIN);

  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == HIGH) {
        ledState = !ledState;
        Serial.print("button pressed");
        Serial.println(ledState);
      }
    }
  }

  if (!ledState) {
    lightning();
  }
  else {
    showPalette();
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButtonState = reading;
}

void lightning() {
  if(flashCounter == 0) {
    dimmer = 5; // the brightness of the leader is scaled down by a factor of 5
  }
  else {
    dimmer = random8(1, 3); // return strokes are brighter than the leader
  }

  FastLED.showColor(CHSV(255, 0, 255/dimmer));
  delay(random8(4, 10)); // each flash only lasts 4-10 milliseconds
  FastLED.showColor(CHSV(255, 0, 0));
  
  if (flashCounter == 0) {
    delay (150); // longer delay until next flash after the leader
  }
  else {
    delay(random8(100)); // shorter delay between strikes  
  }

  if (flashCounter < cycle) {
    flashCounter++;
  }
  else {
    flashCounter = 0;
  }

  delay(random8(FREQUENCY) * 100); // delay between strikes
}

void showPalette() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 1; /* motion speed */
  
  FillLEDsFromPaletteColors(startIndex);  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors(uint8_t colorIndex)
{
  uint8_t brightness = 255;
  
  for( int i = 0; i < NUM_LEDS; ++i) {
      leds[i] = ColorFromPalette(currentPalette, colorIndex, brightness, currentBlending);
      colorIndex += 3;
  }
}

void fillColor() {
  Serial.println(selected);
  Serial.println(colorOptions[selected]);
  fill_solid(leds, NUM_LEDS, colorOptions[selected]);
  FastLED.show();
}
