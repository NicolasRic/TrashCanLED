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

WiFiMulti wifiMulti;
StaticJsonDocument<JSON_ARRAY_SIZE(10)> doc;

#define REDPIN 13
#define GREENPIN 12
#define BLUEPIN 14

#define RED 0
#define GREEN 1
#define BLUE 2

#define PWM_FREQ 5000
#define RESOLUTION 8

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

void updateLED(String bright);

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
  
  if(httpCode == HTTP_CODE_OK) {
    Serial.println("[HTTP] REQUEST OK ");

    updateLED(http.getString());

  } else {
    Serial.println("[HTTP] REQUEST ERROR: " + String(http.GET()));
  }

  http.end();
  
  delay(5000);

  /*
  Serial.println("Going to sleep, will wake in: " + String(CheckRate) + " Seconds.");
  
  delay(5000);

  //Note pin will be turned off on sleep
  esp_sleep_enable_timer_wakeup(CheckRate * 1000000);
  esp_deep_sleep_start();
  */

}

/*
Updates the LED PWM frquency
Takes in a JSON string with the different colours
*/
void updateLED(String bright) {
  deserializeJson(doc, bright);

  ledcWrite(RED, doc[0]["R"]);
  ledcWrite(GREEN, doc[0]["G"]);
  ledcWrite(BLUE, doc[0]["B"]);
}