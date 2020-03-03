/*
Name:    ESP32 led trash can
Author:  Nic Ricci
Website: teamriot.ca
Desc:    Checks evey x time and updates the leds connected
*/

#include "configFile.h"
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define REDPIN 13
#define GREENPIN 12
#define BLUEPIN 14

#define RED 0
#define GREEN 1
#define BLUE 2

#define PWM_FREQ 5000
#define RESOLUTION 8

#define MAXJSONLENGTH 10

WiFiMulti wifiMulti;
const int capacity = 2 * JSON_OBJECT_SIZE(3);
StaticJsonDocument<JSON_ARRAY_SIZE(MAXJSONLENGTH) + MAXJSONLENGTH*JSON_OBJECT_SIZE(3)> doc;
float curr[3];
int buffer[10][3];
String colour;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  //Adds the accesspoint to connect to
  wifiMulti.addAP(SSIDName, SSIDPass);

  //Setup pins
  ledcSetup(RED, PWM_FREQ, RESOLUTION);
  ledcSetup(GREEN, PWM_FREQ, RESOLUTION);
  ledcSetup(BLUE, PWM_FREQ, RESOLUTION);

  ledcAttachPin(REDPIN, RED);
  ledcAttachPin(GREENPIN, GREEN);
  ledcAttachPin(BLUEPIN, BLUE);
}

void updateLED(DynamicJsonDocument bright);
float jsonSubtract(String a, String b);
float jsonMultiply(String a, float b);

void loop() {
  //Waits until connected
  while(wifiMulti.run() != WL_CONNECTED) {
    Serial.println("WiFi not connected, wait...");
    delay(1000);
  }
  
  Serial.println("WiFi connected");
  delay(500);

  HTTPClient http;
  
  http.begin(RestAPI);
  int httpCode = http.GET();
  
  //Updates the colour variable
  if(httpCode == HTTP_CODE_OK) {
    Serial.println("[HTTP] REQUEST OK ");
    colour = http.getString();
    deserializeJson(doc, colour);
  } else {
    Serial.println("[HTTP] REQUEST ERROR: " + String(http.GET()));
  }

  http.end();
  
  for(int i = 0; i < doc.size(); i++) {
    buffer[i][0] = (doc[i]["R"]);
    buffer[i][1] = (doc[i]["G"]);
    buffer[i][2] = (doc[i]["B"]);

    Serial.println(buffer[i][0]);
  }

  curr[0] = buffer[0][0];
  curr[1] = buffer[0][1];
  curr[2] = buffer[0][2];

  //Calculates the gradient between the input values
  for(int i = 0; i < doc.size() - 1; i++) {
    float m[3] = {0,0,0}; //The deltas for RGB

    m[0] = buffer[i+1][0] - buffer[i][0];
    m[1] = buffer[i+1][1] - buffer[i][1];
    m[2] = buffer[i+1][2] - buffer[i][2];

    //Finds largest change
    int divisions = 0;
    for(int k = 0; k < 3; k++) {
      if(m[k] > divisions) {
        divisions = m[k];
      }
    }
    
    divisions = divisions / 5;

    //Calculates the slope
    for(int k = 0; k < 3; k++) {
      m[k] = m[k] / divisions;
      Serial.println(m[k]);
    }

    //Calculates the new RGB values
    for(int j = 0; j < divisions; j++) {
      
      curr[0] = curr[0] + m[0];
      curr[1] = curr[1] + m[1];
      curr[2] = curr[2] + m[2];

      Serial.println("------");
      Serial.println(curr[0]);
      Serial.println(curr[1]);
      Serial.println(curr[2]);

      updateLED(curr);
      delay(200);
    }

    //Sets to i + 2 values
    curr[0] = buffer[i + 1][0];
    curr[1] = buffer[i + 1][1];
    curr[2] = buffer[i + 1][2];
    
    updateLED(curr);
    delay(1000);
  }

  delay(5000);
}

/*
Updates the LED PWM frquency
Takes in a JSON string with the different colours
*/
void updateLED(float bright[3]) {
  ledcWrite(RED, bright[0]);
  ledcWrite(GREEN, bright[1]);
  ledcWrite(BLUE, bright[2]);
}