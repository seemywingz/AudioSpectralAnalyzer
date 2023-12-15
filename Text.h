#ifndef TEXT_H
#define TEXT_H

#include <Adafruit_NeoMatrix.h>

#include "Colors.h"

int textSize = 1;                  // Default text size
int textSpeed = 60;                // Default speed
int textPixelSize = 5;             // Number of pixels per character
String text = "*.*. LuciFi .*.*";  // Default text

void scrollText(Adafruit_NeoMatrix* matrix, String text, ESPWiFi* wifi) {
  matrix->setTextColor(pixelColor);
  matrix->fillScreen(0);
  int startX = matrix->width();
  int len = text.length() * 5;  // Approx width of a character
  for (int x = startX; x > -len; x--) {
    matrix->fillScreen(0);
    matrix->setCursor(x, 0);
    matrix->print(text);
    matrix->show();
    delay(100 - textSpeed);
    if (wifi != nullptr) {
      wifi->handleClient();
    }
  }
}

void staticText(Adafruit_NeoMatrix* matrix, String text) {
  matrix->fillScreen(0);
  matrix->setTextColor(pixelColor);
  matrix->setTextSize(textSize);
  matrix->setCursor(0, 0);
  matrix->print(text);
  matrix->show();
}

void displayOrScrollText(Adafruit_NeoMatrix* matrix, String text,
                         ESPWiFi* wifi) {
  int textLength = text.length() * textPixelSize;
  if (textLength > matrix->width()) {
    scrollText(matrix, text, wifi);  // Scroll text if it's too long
  } else {
    staticText(matrix, text);  // Display static text if it fits
  }
}

void waveText(Adafruit_NeoMatrix* matrix, String text) {
  matrix->setTextColor(pixelColor);  // Set the text color
  int textLength = text.length() * textPixelSize;
  for (int x = 0; x < matrix->width() + textLength; x++) {
    int y = sin(x / 2.0) * 4;  // Sine wave for vertical position
    matrix->fillScreen(0);     // Clear the matrix
    matrix->setCursor(matrix->width() - x, y);  // Set start position
    matrix->print(text);                        // Print the text
    matrix->show();                             // Show on the matrix
    delay(100 - textSpeed);                     // Delay for scrolling speed
  }
}
void blinkText(Adafruit_NeoMatrix* matrix, String text) {
  int startX = matrix->width();
  int textLength = text.length() * textPixelSize;
  bool isVisible = true;  // To toggle visibility
  for (int x = startX; x > -textLength; x--) {
    matrix->fillScreen(0);
    matrix->setCursor(x, 0);
    if (isVisible) {
      matrix->print(text);
    }
    matrix->show();
    delay(100 - textSpeed);
    isVisible = !isVisible;  // Toggle the visibility
  }
}
void rainbowText(Adafruit_NeoMatrix* matrix, String text) {
  int startX = matrix->width();
  int textLength = text.length() * textPixelSize;
  int hue = 0;
  for (int x = startX; x > -textLength; x--) {
    matrix->fillScreen(0);
    matrix->setCursor(x, 0);
    // Use HSV to cycle through colors
    matrix->setTextColor(matrix->ColorHSV(hue));
    matrix->print(text);
    matrix->show();
    delay(100 - textSpeed);  // Adjust speed based on your preference
    hue += 255;
    // Adjust the hue increment for a faster/slower color change
  }
}
#endif
