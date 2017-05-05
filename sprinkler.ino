#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include <FS.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <SPIFFSEditor.h>
#include <AsyncJson.h>
#include <ArduinoJson.h>

// SKETCH BEGIN
AsyncWebServer server(80);

const char* ssid = "xxx";
const char* password = "xxx";
const char * hostName = "sprinkler";
const char* http_username = "admin";
const char* http_password = "admin";

int station1 = 1; //TX
int station2 = 3; 
int station3 = 15;
int station4 = 13;
int station5 = 12;
int station6 = 14; 
int station7 =  2; //RX
int station8 = 0;

unsigned long currentTime;
unsigned long runInterval[7] = {0, 0, 0, 0, 0, 0, 0};
unsigned long offTime[7] = {0, 0, 0, 0, 0, 0, 0};
int stations[] = {station1, station2, station3, station4, station5, station6, station7, station8};



void setup(){

  // preparing GPIOs
  pinMode(station1, OUTPUT);
  pinMode(station2, OUTPUT);
  pinMode(station3, OUTPUT);
  pinMode(station4, OUTPUT);
  pinMode(station5, OUTPUT);
  pinMode(station6, OUTPUT);
  pinMode(station7, OUTPUT);
  pinMode(station8, OUTPUT);
  
  digitalWrite(station1, HIGH);
  digitalWrite(station2, HIGH);
  digitalWrite(station3, HIGH);
  digitalWrite(station4, HIGH);
  digitalWrite(station5, HIGH);
  digitalWrite(station6, HIGH);
  digitalWrite(station7, HIGH);
  digitalWrite(station8, HIGH);
  
  WiFi.hostname(hostName);  
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(hostName);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }

  ArduinoOTA.setHostname(hostName);
  ArduinoOTA.begin();

  SPIFFS.begin();  

  server.addHandler(new SPIFFSEditor(http_username,http_password));  
    
  server.on("/station_status", [](AsyncWebServerRequest *request){
      AsyncResponseStream *response = request->beginResponseStream("text/plain");

     StaticJsonBuffer<512> jsonBuffer;
     JsonObject& root = jsonBuffer.createObject(); 
     JsonArray& offTimer = root.createNestedArray("offTimer");
     JsonArray& stationOn = root.createNestedArray("stationOn");
        
     currentTime = millis();
     for (int i=0; i<8; i++){ 
       if(i<7){
         if (offTime[i]>currentTime){     
          offTimer.add(offTime[i] - currentTime);      
         }
         else {
          offTimer.add(0);
         }
       }
       
       if (digitalRead(stations[i]) == HIGH) {
         stationOn.add(0);
       } else {
         stationOn.add(1);
       }       
     }  
          
     root.printTo(*response); //for debugging
             
     request->send(response);
   }); 

  server.on("/manual_control", HTTP_POST, [](AsyncWebServerRequest *request){ //will only work if a POST is received, HTTP_GET & HTTP_ANY are other options
      
    AsyncResponseStream *response = request->beginResponseStream("text/plain");  //sends respons as plain text, since this is just for JSON data
   
   if(request->hasParam("json", true)){  // looking for a "json=xxx" response, and the true indicates it would be a POST
     AsyncWebParameter* p = request->getParam("json", true);      
     DynamicJsonBuffer jsonBuffer(512);
     JsonObject& root = jsonBuffer.parse( p->value().c_str() );  //puts the value of "json" into the Json Object for parsing    
     //root.printTo(*response); //for debugging

     currentTime = millis();
     for (int i=0; i<7; i++){      
      runInterval[i] = root["timers"][i];
      offTime[i] = runInterval[i] + currentTime;      
     }

    /* for (int i=0; i<7; i++){
       response->println(runInterval[i]);
     }*/ //for debugging
   }
   request->send(response);
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

  server.onNotFound([](AsyncWebServerRequest *request){

    request->send(404, "text/plain", String("hello"));
  });  

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.htm");

  server.onNotFound([](AsyncWebServerRequest *request){

    request->send(404, "text/plain", String("hello"));
  });  
  
  server.begin();
}

void loop(){
  ArduinoOTA.handle();
  
  currentTime = millis();  
  
  for (int i=0; i<7; i++) {   
    
    if (currentTime < offTime[i]) {     
      digitalWrite(stations[i], LOW);           
      
    } else {
       digitalWrite(stations[i], HIGH);          
    }  
  }

  for (int i=0; i<7; i++) {       
    if (digitalRead(stations[i]) == LOW) {
        digitalWrite(station8, LOW);
        break;
       } else {
        digitalWrite(station8, HIGH);
       } 
  }
}
