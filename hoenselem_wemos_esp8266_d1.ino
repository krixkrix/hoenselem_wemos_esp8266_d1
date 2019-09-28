#include <SD.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Config.h"

/*
Please create a file Secrets.h with your Wifi Credentials, like this:
const char* ssid = "YOUR-SSID";
const char* password = "YOUR_PASSWORD";

DO NOT submit the file to GIT
*/
#include "Secrets.h"

WiFiUDP ntpUDP;
int WifiTimeoutSeconds = 20;

/**
 * NTP setup
 */
const char* poolServerName = "0.dk.pool.ntp.org";
const int UPDATE_INTERVAL_HOURS = 5;
const int TIMEZONE_OFFSET_HOURS = 2;
NTPClient timeClient(ntpUDP, poolServerName, TIMEZONE_OFFSET_HOURS*3600, UPDATE_INTERVAL_HOURS*60*60*1000);

// Configuration synced from web
Config config;

// working variables
int minutes_previous = -1;


void setup()
{
  setupLEDs();
  
  Serial.begin(115200);
  
  connectWifi();
  config.sync();

  setGreen(HIGH);
  setRed(HIGH);
  timeClient.begin();
  setGreen(LOW);
  setRed(LOW);

  timeClient.forceUpdate();
  Serial.println();
  Serial.print("Time is: ");
  Serial.println(timeClient.getFormattedTime());
  // sometimes the first timestamp is wrong...?
  delay(200);  
  timeClient.update();
  Serial.println();
  Serial.print("Time is: ");
  Serial.println(timeClient.getFormattedTime());

  doorStateInit();

  // TEST CODE TO INJECT TIME TRIGGERS
  /*
  config.open_hour = timeClient.getHours();
  config.close_hour = config.open_hour;
  config.open_minutes = timeClient.getMinutes() + 1;
  config.close_minutes = config.open_minutes + 2;
  */

  char buf[50];
  sprintf(buf, "Now: %s, Open: %02d:%02d, Close: %02d:%02d", timeClient.getFormattedTime().c_str(), config.open_hour, config.open_minutes, config.close_hour, config.close_minutes);
  ifttt_webhook("Board initialized", "ok", buf);
  
}

void connectWifi() {
  if ( WiFi.status() != WL_CONNECTED ) {
    Serial.println("Connecting WiFi");
    WiFi.begin(ssid, password);

    int i = 0;
    const int delayMs = 200;
    while ( WiFi.status() != WL_CONNECTED ) 
    {
      delay (delayMs);
      toggleRedAndGreen();
      Serial.print ( "." );
      i++;
      if (i*delayMs/1000 > WifiTimeoutSeconds)
      {
        // give up
        break;
      }
    }
  }
  if ( WiFi.status() == WL_CONNECTED ) {
    Serial.println("Wifi OK");
  }
  else {
    Serial.println("Wifi FAILED");
  }
}


void loop() 
{
  timeClient.update();

  int minutes = timeClient.getMinutes();

  if (minutes != minutes_previous) 
  {
    Serial.print("Time is: ");
    Serial.println(timeClient.getFormattedTime());
    
    minutes_previous = minutes;
    int hours = timeClient.getHours();

    if (config.open_hour == hours && config.open_minutes == minutes) 
    {
      doorOpen();
    }
    else if (config.close_hour == hours && config.close_minutes == minutes) 
    {
      doorClose();
    }

    if (hours%3 == 0 && minutes == 0) 
    { 
      ifttt_webhook("Board status", "ok", timeClient.getFormattedTime().c_str());
    }
  }

  if (doorButtonPressed())
  {
    static int n = 1;
    Serial.println("Button press");

    if (doorIsMoving())
    {
      stopMove();
      setGreen(LOW);
      setRed(LOW);
    }
    else 
    {
      if (doorIsOpen())
      {
        Serial.println("door-Is-Open, so close");
        doorClose();
      }
      else if (doorIsClosed())
      {
        Serial.println("door-Is-Closed, so open");
        doorOpen();
      }
      else 
      {
        // move opposite latest direction
        if (getDoorLatestDirection() > 0) 
        {
          doorClose();
        }
        else
        {
          doorOpen();
        }
      }
    }
    delay(300);
  }

/*
  if (doorIsClosed())
  {
    Serial.println("Door closed");
  }
  if (doorIsOpen())
  {
    Serial.println("Door open");
  }
  */
  
  delay(100);
}
