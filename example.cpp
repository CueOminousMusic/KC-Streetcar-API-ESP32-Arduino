#include "touchscreenConfig.h" //https://github.com/RuiSantosdotme/ESP32-TFT-Touchscreen
#include "KCSCApi.h"



//Define Variables
//WiFi Setup
const char* ssid = "***REMOVED***";          // Your WiFi SSID
const char* password = "***REMOVED***";  // Your WiFi Password

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

  tft.drawCentreString("Kansas City Streetcar Arrivals", centerX, 15, FONT_SIZE*1.6);

  //END TOUCHSCREEN SECTION

  //time
  waitForSync();


  delay(10000);  //This is only here to avoid calling the API if the unit restarts quickly
}


void loop() {
  // put your main code here, to run repeatedly:
  int rc = getNow(apistruct);

  //Print some results to serial
  Serial.print("Station: ");
  Serial.println(apistruct.stopName);
  Serial.print("Next Train arriving in: ");
  Serial.print(apistruct.nextMin);
  Serial.println(" minutes");

  Serial.print("Subsequent Train arriving in: ");
  Serial.print(apistruct.secondMin);
  Serial.println(" minutes");


  //begin writing results to screen
  // Clear the screen before writing to it
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  //Title Block
  tft.drawCentreString("KC Streetcar Arrivals", centerX, 15, FONT_SIZE*2);
  //Station Name
  tft.drawCentreString(apistruct.stopName, centerX, 45, FONT_SIZE*1.4);
  //in initial demo, FONT_SIZE = 2 - we're making it just a scoach bigger; 
  tft.drawString("Trains arriving in: ", 45, 80, FONT_SIZE);

  String Arrival = String(apistruct.nextMin) + " minutes"; //this is done on a seperate line to remove ambiguity of String conversion method
  tft.drawString(Arrival, 45, 110, FONT_SIZE*2);
  tft.drawString(apistruct.nextTime12, 190, 110, FONT_SIZE*2);
  String Arrival2 = String(apistruct.secondMin) + " minutes";
  tft.drawString(Arrival2, 45, 150, FONT_SIZE*2);
  tft.drawString(apistruct.secondTime12, 190, 150, FONT_SIZE*2);

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



