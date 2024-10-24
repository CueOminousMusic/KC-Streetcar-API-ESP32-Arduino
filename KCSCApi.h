//This is an attempt to port prairielandelec's KCSCApi.py to c++ for arduino use
#include <stdio.h>
#include <Arduino.h>
#include <WiFi.h>
#include <TimeLib.h>


int    HTTP_PORT   = 80;
std::string HTTP_METHOD = "GET"; // or "POST"

class KCSCApiReturn {
    public:
        int stopID; //the set stopId for the given request
        int predictIndex; //the offset from the next car (1 would be the car after the next car)
        String stopName; //the APIs name for the stop
        float nextTime; //a timestamp in UNIX Epoch time to do math with (in Kansas City Time)
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

        KCSCApiReturn(){
            stopID = 1615;
            predictIndex = 1;
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
        }
};

//construct
KCSCApiReturn apistruct;

//note: below example was 10/20/24 during addition of midtown track - busses only.
char exampleReturn[] = \
"{\"success\":true,\"route\":\"/real-time/kcata/predictions GET\",\"data\":{\n" \
"\"agencyKey\":\"kcata\",\"predictionsData\":[{\n" \
"\"routeShortName\":\"601\",\"routeName\":\"601 - Streetcar\",\"routeId\":\"601\",\"stopId\":\"1615\",\"stopName\":\n" \
"\"CITY MARKET ON WALNUT AT 5TH ST EB\",\"stopCode\":1615,\"destinations\":[{\n" \
"\"directionId\":\"0\",\"headsign\":\"RIVER MARKET NORTH ON 3RD ST AT GRAND WB\",\"predictions\":[{\n" \
"\"time\":1729481495,\"sec\":1031,\"min\":17,\"blockId\":\"6134\",\"occupancyCount\":0,\"occupancyPercent\":20,\"occupancyStatus\":\"MANY_SEATS_AVAILABLE\",\"vehicleId\":\"4000\",\"tripId\":\"55910\"\n" \
"}]}]}]}}\n";   


int getNow(class KCSCApiReturn& apistruct) {
    Serial.println("In getNow function");
    String api_url = "https://kcata-transit.citypost.us/predictions?stopId=" + String(apistruct.stopID) + "&predictions=2";
    Serial.println(api_url);
    String ReturnedString = httpGETRequest(api_url, citypostRootCACert); 
    //Serial.println(ReturnedString);
    JsonDocument ReturnedJSON;
    deserializeJson(ReturnedJSON,ReturnedString);
    //int rc = ReturnedJSON["status_code"];
    JsonDocument usefulSubset = ReturnedJSON["data"]["predictionsData"][0];

    //if (rc != 200) {
    if (ReturnedJSON["success"] == false) {
        Serial.print("Unsuccessful API Fill");
        return 200;
    }
    else{
        //add some code here to separate northbound from southbound. Irrelevant for RM loop, but important elsewhere.
        String stopName = usefulSubset["stopName"];  //String conversion is ambiguous; this two-line method fixes it. 
        apistruct.stopName = stopName;
        apistruct.nextTime = float(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["time"]);
        //nextTimeDT, *12 are calculated, rework this section later.   
        //String DT = ReturnedJSON["nextTimeDT"]; //going direct from JSON to String has several options
        //apistruct.nextTimeDT = DT;
        apistruct.nextTimeDT = 
        //String NT12 = ReturnedJSON["nextTime12"]; //treating other strings like DT
        //apistruct.nextTime12 = NT12;
        apistruct.nextSecs = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["sec"]);
        apistruct.nextMin = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["min"]);
        //apistruct.nextDeparture = bool(ReturnedJSON["nextDeparture"]); //PrairieElec skips this.
        apistruct.nextOccupancy = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["occupancyPercent"]);
        apistruct.nextOccupancyCount = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["occupancyCount"]);
        String NOS = usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["occupancyStatus"];
        apistruct.nextOccupancyStatus = NOS;
        apistruct.nextTripId =int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["occupancyCount"]);
        apistruct.nextVehicleId = int(usefulSubset["destinations"][0]["predictions"][int(apistruct.predictIndex)]["occupancyCount"]);
        if(apistruct.nextSecs < 0) {
          apistruct.isLate = true;
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
          //Serial.println(payload);
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