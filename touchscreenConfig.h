#include <SPI.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>


TFT_eSPI tft = TFT_eSPI();

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

// Print Touchscreen info about X, Y and Pressure (Z) on the Serial Monitor
void printTouchToSerial(int touchX, int touchY, int touchZ) {
  Serial.print("X = ");
  Serial.print(touchX);
  Serial.print(" | Y = ");
  Serial.print(touchY);
  Serial.print(" | Pressure = ");
  Serial.print(touchZ);
  Serial.println();
}

// Print Touchscreen info about X, Y and Pressure (Z) on the TFT Display
void printTouchToDisplay(int touchX, int touchY, int touchZ) {
  // Clear TFT screen
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  int centerX = SCREEN_WIDTH / 2;
  int textY = 80;
 
  String tempText = "X = " + String(touchX);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  textY += 20;
  tempText = "Y = " + String(touchY);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);

  textY += 20;
  tempText = "Pressure = " + String(touchZ);
  tft.drawCentreString(tempText, centerX, textY, FONT_SIZE);
}