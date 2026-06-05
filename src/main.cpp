#include <Arduino.h>
#include <STM32FreeRTOS.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN   PA7
#define NUM_LEDS  30

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);
SemaphoreHandle_t xLedMutex;


void taskRainbow(void *pvParameters) {
  uint8_t offset = 0;
  for (;;) {
    if (xSemaphoreTake(xLedMutex, portMAX_DELAY) == pdTRUE) {
      for (int i = 0; i < NUM_LEDS; i++) {
        uint16_t hue = ((i * 65536L / NUM_LEDS) + offset * 256) & 0xFFFF;
        strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(hue)));
      }
      strip.show();
      xSemaphoreGive(xLedMutex);
    }
    offset++;
    vTaskDelay(pdMS_TO_TICKS(30));
  }
}


void taskComet(void *pvParameters) {
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(2000));
    for (int pos = 0; pos < NUM_LEDS + 10; pos++) {
      if (xSemaphoreTake(xLedMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
        strip.clear();
        for (int i = 0; i < 10; i++) {
          int idx = pos - i;
          if (idx >= 0 && idx < NUM_LEDS) {
            uint8_t bright = 255 - (i * 25);
            strip.setPixelColor(idx, strip.Color(0, bright, bright));
          }
        }
        strip.show();
        xSemaphoreGive(xLedMutex);
      }
      vTaskDelay(pdMS_TO_TICKS(40));
    }
  }
}


void taskPulse(void *pvParameters) {
  uint8_t brightness = 0;
  int8_t  dir = 1;
  for (;;) {
    vTaskDelay(pdMS_TO_TICKS(3000));
    if (xSemaphoreTake(xLedMutex, pdMS_TO_TICKS(20)) == pdTRUE) {
      strip.clear();
      for (int i = 0; i < NUM_LEDS; i++) {
        strip.setPixelColor(i, strip.Color(brightness, 0, brightness / 2));
      }
      strip.show();
      xSemaphoreGive(xLedMutex);
    }
    brightness += dir * 5;
    if (brightness >= 250 || brightness <= 0) dir *= -1;
    vTaskDelay(pdMS_TO_TICKS(15));
  }
}

void setup() {
  strip.begin();
  strip.setBrightness(50);
  strip.clear();
  strip.show();

  xLedMutex = xSemaphoreCreateMutex();

  xTaskCreate(taskRainbow, "Rainbow", 512, NULL, 2, NULL);
  xTaskCreate(taskComet,   "Comet",   512, NULL, 1, NULL);
  xTaskCreate(taskPulse,   "Pulse",   512, NULL, 1, NULL);

  vTaskStartScheduler();
}

void loop() {}