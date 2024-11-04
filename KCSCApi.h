//This is an attempt to port prairielandelec's KCSCApi.py to c++ for arduino use
#include <Arduino.h>
#include <stdio.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h> //https://arduinojson.org
#include <ezTime.h> //https://github.com/ropg/ezTime

#include "citypostCert.h"

//Declarations
String httpGETRequest(String serverName, const char * rootCACertificate);
int getNow(class KCSCApiReturn & apistruct);
String prependZero(int);
Timezone tzKC;


int    HTTP_PORT   = 80;
std::string HTTP_METHOD = "GET"; // or "POST"







//Example API Return: https://kcata-transit.citypost.us/predictions?stopId=1615
//note: below example was 10/20/24 during addition of midtown track - busses only.
char exampleReturn[] = \
"{\"success\":true,\"route\":\"/real-time/kcata/predictions GET\",\"data\":{\n" \
"\"agencyKey\":\"kcata\",\"predictionsData\":[{\n" \
"\"routeShortName\":\"601\",\"routeName\":\"601 - Streetcar\",\"routeId\":\"601\",\"stopId\":\"1615\",\"stopName\":\n" \
"\"CITY MARKET ON WALNUT AT 5TH ST EB\",\"stopCode\":1615,\"destinations\":[{\n" \
"\"directionId\":\"0\",\"headsign\":\"RIVER MARKET NORTH ON 3RD ST AT GRAND WB\",\"predictions\":[{\n" \
"\"time\":1729481495,\"sec\":1031,\"min\":17,\"blockId\":\"6134\",\"occupancyCount\":0,\"occupancyPercent\":20,\n" \
"\"occupancyStatus\":\"MANY_SEATS_AVAILABLE\",\"vehicleId\":\"4000\",\"tripId\":\"55910\"\n" \
"}]}]}]}}\n"; 



class KCSCApiReturn {
    public:
        int stopID; //the set stopId for the given request
        int predictIndex; //the offset from the next car (1 would be the car after the next car)
        //JSON counts from zero, but API query string seems to count from 1
        String stopName; //the APIs name for the stop
        unsigned long nextTime; //a timestamp in UNIX Epoch time to do math with - UTC
        String nextTimeDT; //datetime object - calculated. Not implemented yet.
        String nextTime12; //calculated. not implemented yet.
        int nextSecs; // a nice small integer to do more basic math with
        int nextMin; //same as above, but in minutes
        bool nextDeparture; //suspects it chages when departure happens
        int nextOccupancy; //percentage occupancy - source?
        int nextOccupancyCount; //number of riders - is this real?
        String nextOccupancyStatus; //String
        int nextTripId; //segment of the next car - what does that mean?
        int nextVehicleId; //train id
        bool isLate; //calculated value
        //Might be better to create a better data structure for flexible sizing. But you can pull both in one API call with an extra param
        //int
        unsigned long secondTime; //adding second timestamp
        String secondTime12;
        int secondSecs; // adding a second time.  
        int secondMin; //same as above, but in minutes

        KCSCApiReturn(){
            stopID = 1615;
            predictIndex = 1; //0 returns 1 prediction, 1 returns 2, etc. 
            stopName = "CITY MARKET ON WALNUT AT 5TH ST Eastbound";
            nextTime = 000000000;
            nextTimeDT = "";
            nextTime12 = "";
            nextSecs = 0000;
            nextMin = 000;
            nextDeparture = false;
            nextOccupancy = 00;
            nextOccupancyCount = 000;
            nextOccupancyStatus = "";
            nextTripId =00000;
            nextVehicleId = 0000;
            isLate = false; 
            secondTime = 000000000;
            secondSecs = 0000;
            secondMin = 000;
        }
};

//construct
KCSCApiReturn apistruct;


//Functions
int getNow(class KCSCApiReturn& apistruct) {
    Serial.println("In getNow function");
    String api_url = "https://kcata-transit.citypost.us/predictions?stopId=" + String(apistruct.stopID) + "&predictions=" +String(apistruct.predictIndex); 
    Serial.println(api_url);
    String ReturnedString = httpGETRequest(api_url, citypostRootCACert); 
    JsonDocument ReturnedJSON;
    deserializeJson(ReturnedJSON,ReturnedString);
    JsonDocument usefulSubset = ReturnedJSON["data"]["predictionsData"][0];

    if (ReturnedJSON["success"] == false) {
        Serial.print("Unsuccessful API Fill");
        return 200;
    }
    else{
        //add some code here to clarify northbound from southbound. Irrelevant for RM loop, but maybe important elsewhere.
        String stopName = usefulSubset["stopName"];  //String conversion is ambiguous; this two-line method fixes it. 
        apistruct.stopName = stopName;
        apistruct.nextTime =  long(usefulSubset["destinations"][0]["predictions"][0]["time"]);
        tzKC.setLocation("America/Chicago");
        int tzAdj = tzKC.getOffset()*60;
        Serial.println(tzAdj);
        Serial.println(apistruct.nextTime-tzAdj);

        //nextTimeDT is caclulated - no implementation yet.   
        //apistruct.nextTimeDT = NULL;
        //nextTime is an epoch; need to adjust to Central and ensure we don't lose leading zeros on minutes
        //Need to adjust for daylight savings time - there's probably a better way to do this with the new library. 
        String NT12 = String(hourFormat12(apistruct.nextTime-tzAdj, UTC_TIME)) + ":" + zeropad(minute(apistruct.nextTime, UTC_TIME),2);
        Serial.println(NT12);
        apistruct.nextTime12 = NT12;
        apistruct.nextSecs = int(usefulSubset["destinations"][0]["predictions"][0]["sec"]);
        apistruct.nextMin = int(usefulSubset["destinations"][0]["predictions"][0]["min"]);
        //apistruct.nextDeparture = bool(ReturnedJSON["nextDeparture"]); //PrairieElec skips this.
        apistruct.nextOccupancy = int(usefulSubset["destinations"][0]["predictions"][0]["occupancyPercent"]);
        apistruct.nextOccupancyCount = int(usefulSubset["destinations"][0]["predictions"][0]["occupancyCount"]);
        String NOS = usefulSubset["destinations"][0]["predictions"][0]["occupancyStatus"];
        apistruct.nextOccupancyStatus = NOS;
        apistruct.nextTripId =int(usefulSubset["destinations"][0]["predictions"][0]["occupancyCount"]);
        apistruct.nextVehicleId = int(usefulSubset["destinations"][0]["predictions"][0]["occupancyCount"]);
        if(apistruct.nextSecs < 0) {
          apistruct.isLate = true;
        }
        //for (int i =0; i <= apistruct.predictIndex; i++) {
        if (apistruct.predictIndex > 0) {
          apistruct.secondTime = long(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["time"]);
          String ST12 = String(hourFormat12(apistruct.secondTime-tzAdj, UTC_TIME)) + ":" + zeropad(minute(apistruct.secondTime, UTC_TIME),2);
          apistruct.secondTime12 = ST12;
          apistruct.secondSecs = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["sec"]);
          apistruct.secondMin = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["min"]);
        }
        return 200;
    } 
  
}


String httpGETRequest(String serverName, const char * rootCACertificate) {
  WiFiClientSecure *client = new WiFiClientSecure;
  if(client) {
    // set secure client with certificate
    client->setCACert(rootCACertificate);
    //create an HTTPClient instance
    HTTPClient http;

    // Your IP address with path or Domain name with URL path 
    Serial.print("[HTTPS] begin...\n");
    if (http.begin(*client, serverName)) {  // HTTPS
      Serial.print("[HTTPS] GET...\n");
      // start connection and send HTTP header
      int httpCode = http.GET();
      // httpCode will be negative on error
      if (httpCode > 0) {
      // HTTP header has been send and Server response header has been handled
       Serial.printf("[HTTPS] GET... code: %d\n", httpCode);
      // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          // print server response payload
          String payload = http.getString();
          return payload;
        }
      }
      else {
        Serial.printf("[HTTPS] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
        return String(httpCode);
      }
      http.end();
    }
    return "http.begin fail";
  }
  else {
    Serial.printf("[HTTPS] Unable to connect\n");
    return "Client Fail";
  }


}