#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

#define NUM_LEDS  30
#define DATA_PIN  PA7

Adafruit_NeoPixel strip(NUM_LEDS, DATA_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  strip.begin();
  strip.setBrightness(255);
  strip.fill(strip.Color(255, 0, 0));
  strip.show();
}

void loop() {}