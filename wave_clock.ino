#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
const char* ssid = "AndroidAP";
const char* password = "pxlj9479";
const char* api_url = "http://5.100.255.196/waveclock/";
const int httpsPort = 443;
const char fingerprint[] PROGMEM = "ff 68 80 2d 5e 35 c4 dc 61 e3 6b f5 51 51 9e 73 d0 f6 0e f6";
Servo servoWaveHeight;
Servo servoWindDirection;
int angleWaveHeight, angleWindDir;
// const char* api_url = "http://jsonplaceholder.typicode.com/todos/1/";

void setup () {
  Serial.begin(9600);
  WiFi.begin(ssid, password);
  servoWaveHeight.attach(2); //D4
  servoWindDirection.attach(12); //D6
  servoWaveHeight.write(0);
  delay(2000);
  servoWindDirection.write(0);
  delay(2000);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print("Connecting..");  
  }
  Serial.print("Connected to wifi successfuly"); 
}

void loop(){
 
  HTTPClient http;  //Declare an object of class HTTPClient
  http.begin(api_url);
  int httpCode = http.GET();   
  Serial.println(httpCode);
  if (httpCode > 0) { //Check the returning code
    const int capacity = 6 * JSON_OBJECT_SIZE(3) + 3*JSON_OBJECT_SIZE(2);
    StaticJsonDocument<capacity> doc;
    String payload = http.getString();   //Get the request response payload
    DeserializationError jsonerr = deserializeJson(doc, payload);
    
    // print error to console (if any)
    if(jsonerr) {
      Serial.println(payload);
      Serial.print(F("deserializeJson() failed with code "));
      Serial.println(jsonerr.c_str());
    }
    
    // for debugging
    Serial.printf("min wave hieght: %d",doc[0]["swell"]["minBreakingHeight"].as<float>()) ;                     //Print the response payload
    Serial.printf(" | max wave hieght: %d",doc[0]["swell"]["maxBreakingHeight"].as<float>());
    Serial.printf(" | wind direction : %d",doc[0]["wind"]["direction"].as<int>());

    // find servo angle for wave height
    servoWaveHeight.write(findHeightAngle(doc[0]["swell"]["maxBreakingHeight"].as<int>()));
    delay(1000);
    
    // find servo angle for wind direction
    servoWindDirection.write(findWindAngle(doc[0]["wind"]["direction"].as<int>()));
    delay(1000);
    
  }else{
    Serial.println("start error code"); 
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str()); 
    Serial.println("end error code"); 
  }
  http.end();   //Close connection 
  delay(30000);    //Send a request every 30 seconds
}

int findHeightAngle(float api_val){
  int offset =10;
  if(api_val < 2){
    return offset;
  }
  if(api_val >= 2 && api_val<4){
    return 60 + offset;
  }
  if(api_val >4){
    return 120 +offset;
  }
 
}

int findWindAngle(int api_val){
 return map(api_val,0,360,0,180);
}
 
