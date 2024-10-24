#include <Arduino.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> //https://arduinojson.org
#include "touchscreenConfig.h" //https://github.com/RuiSantosdotme/ESP32-TFT-Touchscreen

#include "KCSCApi.h"
#include "citypostCert.h"


//Define Variables
//WiFi Setup
const char* ssid = "";          // Your WiFi SSID
const char* password = "";  // Your WiFi Password

//useful screen math
int centerX = SCREEN_WIDTH / 2;
int centerY = SCREEN_HEIGHT / 2;


// put function declarations here:



void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  //WIFI SECTION
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  //END WIFI SECTION

  //TOUCHSCREEN SECTION
  // Start the SPI for the touchscreen and init the touchscreen
  touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
  touchscreen.begin(touchscreenSPI);
  // Set the Touchscreen rotation in landscape mode
  // Note: in some displays, the touchscreen might be upside down, so you might need to set the rotation to 3: touchscreen.setRotation(3);
  touchscreen.setRotation(1);

  // Start the tft display
  tft.init();
  // Set the TFT display rotation in landscape mode
  tft.setRotation(1);

  // Clear the screen before writing to it
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);

  tft.drawCentreString("Kansas City Streetcar Arrivals", centerX, 30, FONT_SIZE);
  //tft.drawCentreString("Touch screen to test", centerX, centerY, FONT_SIZE);
  //END TOUCHSCREEN

  delay(10000);  //This is only here to avoid calling the API if the unit restarts quickly
}


void loop() {
  // put your main code here, to run repeatedly:
  int rc = getNow(apistruct);
  Serial.print("Station: ");
  Serial.println(apistruct.stopName);
  Serial.print("Next Train arriving in: ");
  Serial.print(apistruct.nextMin);
  Serial.println(" minutes");

//  String StationID = "Station: " + String(apistruct.stopName);
  tft.drawCentreString(apistruct.stopName, centerX, 60, FONT_SIZE);

  String Arrival = "Next Train: " + String(apistruct.nextMin) + " minutes";
  tft.drawCentreString(Arrival, centerX, 90, FONT_SIZE);

  // Checks if Touchscreen was touched, and prints X, Y and Pressure (Z) info on the TFT display and Serial Monitor
  if (touchscreen.tirqTouched() && touchscreen.touched()) {
    // Get Touchscreen points
    TS_Point p = touchscreen.getPoint();
    // Calibrate Touchscreen points with map function to the correct width and height
    x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
    y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
    z = p.z;

    printTouchToSerial(x, y, z);
    //printTouchToDisplay(x, y, z);
  }
  delay(60000);
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}



