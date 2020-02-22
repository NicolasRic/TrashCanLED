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

WiFiMulti wifiMulti;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting...");

  //Adds the accesspoint to connect to
  wifiMulti.addAP(SSIDName, SSIDPass);
}

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
    Serial.println(http.getString());
  } else {
    Serial.println("[HTTP] REQUEST ERROR: " + String(http.GET()));
  }

  http.end();
  
  delay(500);

  Serial.println("Going to sleep, will wake in: " + String(CheckRate) + " Seconds.");
  
  delay(5000);

  esp_sleep_enable_timer_wakeup(CheckRate * 1000000);
  esp_deep_sleep_start();

}
