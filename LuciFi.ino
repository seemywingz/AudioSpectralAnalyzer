#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <ESPWiFi.h>
#include <arduinoFFT.h>

#include "Birds.h"
#include "Colors.h"
#include "GameofLife.h"
#include "Motion.h"
#include "SpectralAnalyzer.h"
#include "Stars.h"
#include "Text.h"

// LED Matrix Config
int LEDWidth = 32;
int LEDHeight = 8;
int ledDataPin = 12;  // Board Pin D6
uint8_t matrixType =
    NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG;
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(
    LEDWidth, LEDHeight, ledDataPin, matrixType, NEO_GRB + NEO_KHZ800);

// Brightness and Color Config
const int maxBrightness = 255;
const int minBrightness = 1;
int brightness = 6;

// Visualization Config
const int maxFrameRate = 120;
unsigned int frameRate = 30;
String visualization = "motion";

// Web Server Config
ESPWiFi wifi = ESPWiFi("Luci-Fi", "abcd1234");

void setup() {
  matrix.begin();
  Serial.begin(115200);
  while (!Serial) {
  };
  matrix.setTextWrap(false);
  matrix.setBrightness(brightness);
  testMatrix(&matrix, LEDWidth, LEDHeight);
  initializeMotion(LEDWidth, LEDHeight);
  initializeWebServer();
}

void loop() {
  wifi.handleClient();
  if (visualization == "waveform") {
    drawWaveform();
  } else if (visualization == "circles") {
    drawCircles();
  } else if (visualization == "birds") {
    runAtFrameRate(drawBirds, frameRate);
  } else if (visualization == "gameOfLife") {
    runAtFrameRate(drawGameOfLife, frameRate);
  } else if (visualization == "motion") {
    // runAtFrameRate(drawMotion, frameRate);
    drawMotion();
  } else if (visualization == "matrix") {
    // the matrix visualization will be a 2d matrix of pixels that will
    // simulate the screen from the matrix movie
    // runAtFrameRate(drawMatrix, frameRate);
  } else if (visualization == "starPulse") {
    drawStarPulse();
  } else {
    drawBars();
  }
}

void drawMotion() {
  motionAnimation(LEDWidth, LEDHeight);
  matrix.fillScreen(0);
  for (int i = 0; i < motionNumObjects; i++) {
    matrix.drawPixel(motionObjects[i].x, motionObjects[i].y,
                     motionObjects[i].color);
  }
  matrix.show();
}

void drawBars() {
  spectralAnalyzer(LEDWidth, LEDHeight);
  matrix.fillScreen(0);
  for (int x = 0; x < LEDWidth; x++) {
    for (int y = 0; y < spectralData[x]; y++) {
      uint32_t pixelColor = colorPallet[0];
      pixelColor = (y > 1) ? colorPallet[1] : pixelColor;
      pixelColor = (y > 3) ? colorPallet[2] : pixelColor;
      pixelColor = (y > 6) ? colorPallet[3] : pixelColor;
      matrix.drawPixel(x, LEDHeight - 1 - y, pixelColor);
    }
  }
  matrix.show();
}

void drawBirds() {
  if (birds == nullptr) {
    generateBirds(LEDWidth, LEDHeight);
  }
  updateFlock(LEDWidth, LEDHeight);
  matrix.fillScreen(0);
  for (int i = 0; i < birdCount; i++) {
    matrix.drawPixel(birds[i].x, birds[i].y, birds[i].color);
  }
  matrix.show();
}

void drawCircles() {
  spectralAnalyzer(LEDWidth, LEDHeight);
  matrix.fillScreen(0);
  for (int x = 0; x < LEDWidth; x++) {
    int circleRadius = spectralData[x] / 2;
    int circleColor = colorPallet[0];
    circleColor = (circleRadius > 2) ? colorPallet[1] : circleColor;
    circleColor = (circleRadius > 3) ? colorPallet[2] : circleColor;
    circleColor = (circleRadius >= 4) ? colorPallet[3] : circleColor;
    if (circleRadius > 0) {
      matrix.drawCircle(x, 4, circleRadius, circleColor);
    }
  }
  matrix.show();
}

void drawGameOfLife() {
  if (gol_Cells == nullptr) {
    startGameOfLife(LEDWidth, LEDHeight);
  }
  updateGameOfLife(LEDWidth, LEDHeight, 231);
  matrix.fillScreen(0);
  for (int x = 0; x < LEDWidth; x++) {
    for (int y = 0; y < LEDHeight; y++) {
      if (gol_Cells[x][y] == 1) {
        matrix.drawPixel(x, y, pixelColor);
      }
    }
  }
  matrix.show();
}

void drawStarPulse() {
  if (stars == nullptr) {
    initializeStars(LEDWidth, LEDHeight);
  }
  spectralAnalyzer(LEDWidth, LEDHeight);
  updateStartPulse(LEDWidth, LEDHeight);
  matrix.fillScreen(0);
  for (int i = 0; i < starCount; i++) {
    matrix.drawPixel(stars[i].x, stars[i].y, stars[i].color);
  }
  matrix.show();
}

void drawWaveform() {
  spectralAnalyzer(LEDWidth, LEDHeight);  // Analyze the audio input
  matrix.fillScreen(0);                   // Clear the matrix

  int middleY = LEDHeight / 2;  // Calculate the middle row of the matrix

  // Drawing a line for each column based on the spectral data
  for (int x = 0; x < LEDWidth; x++) {
    int value = spectralData[x] / 2;  // Get the value for this column
    for (int y = 0; y < value; y++) {
      // Draw upwards from the middle
      uint32_t pixelColor = colorPallet[0];
      if (y > 1) {
        pixelColor = colorPallet[1];
      }
      if (y > 2) {
        pixelColor = colorPallet[2];
      }
      if (y > 3) {
        pixelColor = colorPallet[3];
      }
      matrix.drawPixel(x, middleY - y, pixelColor);
      // Draw downwards from the middle
      if (y > 0 && middleY + y < LEDHeight) {
        matrix.drawPixel(x, middleY + y, pixelColor);
      }
    }
  }

  matrix.show();  // Update the display with the waveform
}

void setBrightness(int newBrightness) {
  brightness = constrain(newBrightness, minBrightness, maxBrightness);
  matrix.setBrightness(brightness);
  matrix.show();
}

void setSensitivity(int newSensitivity) {
  constrain(newSensitivity, minSensitivity, maxSensitivity);
  sensitivity = newSensitivity;
}

void setFramerate(unsigned int fps) {
  frameRate = constrain(fps, 1, maxFrameRate);
}

void initializeWebServer() {
  wifi.webServer.on("/sensitivity", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain", String(sensitivity));
  });
  wifi.webServer.on("/sensitivity", HTTP_POST, []() {
    if (wifi.webServer.hasArg("sensitivity")) {
      int newSensitivity = wifi.webServer.arg("sensitivity").toInt();
      setSensitivity(newSensitivity);
      wifi.webServer.send(200, "text/plain",
                          "Sensitivity updated: " + String(sensitivity) + " %");
    } else {
      wifi.webServer.send(400, "text/plain", "Missing Sensitivity value");
    }
  });

  wifi.webServer.on("/brightness", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain", String(brightness));
  });
  wifi.webServer.on("/brightness", HTTP_POST, []() {
    if (wifi.webServer.hasArg("brightness")) {
      int newBrightness = wifi.webServer.arg("brightness").toInt();
      setBrightness(newBrightness);
      wifi.webServer.send(200, "text/plain",
                          "Brightness updated: " + String(brightness) + " %");
    } else {
      wifi.webServer.send(400, "text/plain", "Missing Brightness value");
    }
  });

  wifi.webServer.on("/motion", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain",
                        "motionNumObjects=" + String(motionNumObjects) + "\n");
  });
  wifi.webServer.on("/motion", HTTP_POST, []() {
    if (wifi.webServer.hasArg("motionNumObjects")) {
      motionNumObjects = wifi.webServer.arg("motionNumObjects").toInt();
    }
    generateMotionObjects(LEDWidth, LEDHeight);
    wifi.webServer.send(200, "text/plain", "Motion settings updated");
  });

  wifi.webServer.on("/starPulse", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain",
                        "starCount=" + String(starCount) + "\n");
  });
  wifi.webServer.on("/starPulse", HTTP_POST, []() {
    if (wifi.webServer.hasArg("starCount")) {
      starCount = wifi.webServer.arg("starCount").toInt();
    }
    initializeStars(LEDWidth, LEDHeight);
    wifi.webServer.send(200, "text/plain", "Star Pulse settings updated");
  });

  wifi.webServer.on("/frameRate", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain", String(frameRate));
  });
  wifi.webServer.on("/frameRate", HTTP_POST, []() {
    if (wifi.webServer.hasArg("frameRate")) {
      unsigned int newFrameRate = wifi.webServer.arg("frameRate").toInt();
      setFramerate(newFrameRate);  // Update the global framerate
      wifi.webServer.send(200, "text/plain",
                          "Frame Rate updated: " + String(frameRate) + " fps");
    } else {
      wifi.webServer.send(400, "text/plain", "Missing Frame Rate value");
    }
  });

  wifi.webServer.on("/visualization", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain", visualization);
  });
  wifi.webServer.on("/visualization", HTTP_POST, []() {
    if (wifi.webServer.hasArg("visualization")) {
      visualization = wifi.webServer.arg("visualization");
      wifi.webServer.send(200, "text/plain",
                          "Visualization set to: " + visualization);
    } else {
      wifi.webServer.send(400, "text/plain", "Missing visualization mode");
    }
  });

  wifi.webServer.on("/colors", HTTP_GET, []() {
    wifi.webServer.send(200, "text/plain",
                        "color1=" + colorToHex(colorPallet[0]) + "\n" +
                            "color2=" + colorToHex(colorPallet[1]) + "\n" +
                            "color3=" + colorToHex(colorPallet[2]) + "\n" +
                            "color4=" + colorToHex(colorPallet[3]) + "\n" +
                            "pixelColor=" + colorToHex(pixelColor) + "\n");
  });
  wifi.webServer.on("/colors", HTTP_POST, []() {
    if (wifi.webServer.hasArg("color1")) {
      String color = wifi.webServer.arg("color1");
      colorPallet[0] = hexToColor(wifi.webServer.arg("color1"));
    }
    if (wifi.webServer.hasArg("color2")) {
      String color = wifi.webServer.arg("color2");
      colorPallet[1] = hexToColor(wifi.webServer.arg("color2"));
    }
    if (wifi.webServer.hasArg("color3")) {
      String color = wifi.webServer.arg("color3");
      colorPallet[2] = hexToColor(wifi.webServer.arg("color3"));
    }
    if (wifi.webServer.hasArg("color4")) {
      String color = wifi.webServer.arg("color4");
      colorPallet[3] = hexToColor(wifi.webServer.arg("color4"));
    }
    if (wifi.webServer.hasArg("pixelColor")) {
      String color = wifi.webServer.arg("pixelColor");
      pixelColor = hexToColor(wifi.webServer.arg("pixelColor"));
    }
    wifi.webServer.send(200, "text/plain",
                        "Color Pallet set to: " + String(colorPallet[0]) +
                            ", " + String(colorPallet[1]) + ", " +
                            String(colorPallet[2]) + ", " +
                            String(colorPallet[3]) + ", " + String(pixelColor));
  });

  wifi.webServer.on("/text", HTTP_POST, []() {
    if (wifi.webServer.hasArg("textColor")) {
      String color = wifi.webServer.arg("textColor");
      pixelColor = hexToColor(color);
    }
    if (wifi.webServer.hasArg("textSpeed")) {
      textSpeed = wifi.webServer.arg("textSpeed").toInt();
      wifi.webServer.send(200, "text/plain", "Speed updated");
    }
    if (wifi.webServer.hasArg("text")) {
      text = wifi.webServer.arg("text");
    }
    if (wifi.webServer.hasArg("textAnimation")) {
      String textAnimation = wifi.webServer.arg("textAnimation");
      if (textAnimation == "scroll") {
        scrollText(&matrix, text);
      } else if (textAnimation == "wave") {
        waveText(&matrix, text);
      } else if (textAnimation == "rainbow") {
        rainbowText(&matrix, text);
      }
    } else {
      scrollText(&matrix, text);
    }
    wifi.webServer.send(200, "text/plain", "Text updated");
  });

  wifi.webServer.on("/birds", HTTP_GET, []() {
    String response = "";
    response += "birdCount=" + String(birdCount) + "\n";
    response += "birdAlignment=" + String(birdAlignment) + "\n";
    response += "birdCohesion=" + String(birdCohesion) + "\n";
    response += "birdSeparation=" + String(birdSeparation) + "\n";
    response += "birdVerticalBounds=" + String(birdVerticalBounds) + "\n";
    response += "birdHorizontalBounds=" + String(birdHorizontalBounds) + "\n";
    wifi.webServer.send(200, "text/plain", response);
  });

  wifi.webServer.on("/birds", HTTP_POST, []() {
    if (wifi.webServer.hasArg("birdCount")) {
      birdCount = wifi.webServer.arg("birdCount").toInt();
    }
    if (wifi.webServer.hasArg("birdAlignment")) {
      birdAlignment = wifi.webServer.arg("birdAlignment").toFloat();
    }
    if (wifi.webServer.hasArg("birdCohesion")) {
      birdCohesion = wifi.webServer.arg("birdCohesion").toFloat();
    }
    if (wifi.webServer.hasArg("birdSeparation")) {
      birdSeparation = wifi.webServer.arg("birdSeparation").toFloat();
    }
    if (wifi.webServer.hasArg("birdVerticalBounds")) {
      birdVerticalBounds =
          wifi.webServer.arg("birdVerticalBounds").compareTo("true") == 0;
    }
    if (wifi.webServer.hasArg("birdHorizontalBounds")) {
      birdHorizontalBounds =
          wifi.webServer.arg("birdHorizontalBounds").compareTo("true") == 0;
    }
    generateBirds(LEDWidth, LEDHeight);
    wifi.webServer.send(200, "text/plain", "Bird settings updated");
  });

  wifi.setConnectSubroutine([]() { testMatrix(&matrix, LEDWidth, LEDHeight); });
  wifi.enableMDNS("luci-fi");
  wifi.start();
}
