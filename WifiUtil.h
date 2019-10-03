#ifndef WIFI_UTIL_H
#define WIFI_UTIL_H

#include "LedUtil.h"

/*
Please create a file Secrets.h with your Wifi Credentials, like this:
const char* ssid = "YOUR-SSID";
const char* password = "YOUR_PASSWORD";

DO NOT submit the file to GIT
*/
#include "Secrets.h"

int WifiTimeoutSeconds = 30;

bool isConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool connectWifi() {
  if ( WiFi.status() != WL_CONNECTED ) {
    Serial.println("Connecting WiFi");
    WiFi.begin(ssid, password);

    int i = 0;
    const int delayMs = 200;
    while ( WiFi.status() != WL_CONNECTED ) 
    {
      toggleRedAndGreen();
      Serial.print ( "." );
      if (i*delayMs/1000 > WifiTimeoutSeconds)
      {
        // give up
        break;
      }
      i++;
      delay (delayMs);
    }
  }
   
  if ( WiFi.status() == WL_CONNECTED ) {
    Serial.println("Wifi OK");
  }
  else {
    Serial.println("Wifi FAILED");
  }
  return WiFi.status() == WL_CONNECTED;
}


#endif
