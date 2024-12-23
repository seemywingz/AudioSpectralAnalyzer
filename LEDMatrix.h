#ifndef LITBOX_LED_MATRIX_H
#define LITBOX_LED_MATRIX_H

#include <FastLED.h>

#include "colors.h"

// LED Matrix Config
#define LED_PIN 12
#define LED_WIDTH 32
#define LED_HEIGHT 8
#define NUM_LEDS (LED_WIDTH * LED_HEIGHT)
#define COLOR_ORDER GRB
#define CHIPSET WS2812B

CRGB leds[NUM_LEDS];

// Brightness
const uint8_t minBrightness = 1;
const uint8_t maxBrightness = 255;
uint8_t brightness = 9;

void initializeMatrix() {
  FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
      .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  FastLED.clear();
  FastLED.show();
  Serial.println("LED Matrix Initialized");
}

uint16_t XY(uint8_t x, uint8_t y) {
  y = LED_HEIGHT - 1 - y;  // Adjust for orientation
  if (x % 2 == 0) {
    return x * LED_HEIGHT + y;  // Even columns top to bottom
  } else {
    return x * LED_HEIGHT + (LED_HEIGHT - 1 - y);  // Odd columns bottom to top
  }
}

void drawPixel(uint8_t x, uint8_t y, CRGB color) {
  uint16_t index = XY(x, y);
  if (index < NUM_LEDS) {
    leds[index] = color;
  }
}

void testMatrix() {
  CRGB testColor = CRGB::White;

  // Light up all LEDs one by one
  for (int x = 0; x < LED_WIDTH; x++) {
    for (int y = 0; y < LED_HEIGHT; y++) {
      drawPixel(x, y, testColor);
      FastLED.show();
      delay(10);
      drawPixel(x, y, CRGB::Black);
    }
  }

  int del = 100;
  // Test specific positions
  FastLED.clear();
  drawPixel(0, 0, testColor);  // Bottom left
  FastLED.show();
  delay(del);

  FastLED.clear();
  drawPixel(0, LED_HEIGHT - 1, testColor);  // Top left
  FastLED.show();
  delay(del);

  FastLED.clear();
  drawPixel(LED_WIDTH - 1, LED_HEIGHT - 1, testColor);  // Top right
  FastLED.show();
  delay(del);

  FastLED.clear();
  drawPixel(LED_WIDTH - 1, 0, testColor);  // Bottom right
  FastLED.show();
  delay(del);

  FastLED.clear();
  drawPixel(LED_WIDTH / 2, LED_HEIGHT / 2, testColor);  // Center
  FastLED.show();
  delay(del);

  FastLED.clear();
  FastLED.show();
}

#endif  // LITBOX_LED_MATRIX_H