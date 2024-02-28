#include <FastLED.h>

#define LED_PIN     3
#define NUM_LEDS    2
#define BRIGHTNESS  16
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS]; 
uint8_t startLed = 0;

void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
}

void loop()
{

}
