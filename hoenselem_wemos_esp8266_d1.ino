// Project intended for board: "WeMos D1 R1"
#include <SD.h>

#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include "Config.h"
#include "IftttReporting.h"
#include "WifiUtil.h"
#include "LedUtil.h"
#include "DoorControl.h"

#define VERSION "v2"

WiFiUDP ntpUDP;

/**
 * NTP setup
 */
const char* poolServerName = "0.dk.pool.ntp.org";
const int UPDATE_INTERVAL_HOURS = 5;
const int TIMEZONE_OFFSET_HOURS = 2;
NTPClient timeClient(ntpUDP, poolServerName, TIMEZONE_OFFSET_HOURS*3600, UPDATE_INTERVAL_HOURS*60*60*1000);

// Configuration synced from web
Config config;
Config configTmp;

// working variables
int minutes_previous = -1;
unsigned long unix_latest_config_update = 0;

void setup()
{
  setupLEDs();
  
  Serial.begin(115200);
  WiFi.mode(WIFI_OFF);  // Prevents reconnection issue (taking too long to connect)
  delay(500);
  WiFi.mode(WIFI_STA);  // Only Station No AP, This line hides the viewing of ESP as wifi hotspot
  delay(500);
  connectWifi();
  
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

  ifttt_webhook("Boot "VERSION, true, timeClient.getFormattedTime().c_str());

  doorStateInit();


  // TEST CODE TO INJECT TIME TRIGGERS
  /*
  config.open_hour = timeClient.getHours();
  config.close_hour = config.open_hour;
  config.open_minutes = timeClient.getMinutes() + 1;
  config.close_minutes = config.open_minutes + 2;
  */

  bool ok = getGoogleConfig(config);
  
  ifttt_webhook("Config init", ok, ok ? config.formatted() : getConfigError().c_str());

  if (ok) 
  {
    unix_latest_config_update = timeClient.getEpochTime();
  }
  else 
  {
    ifttt_webhook("Config fallback", true, config.formatted());
  }
}

unsigned long minutesSinceConfigUpdate() 
{
  return (timeClient.getEpochTime() - unix_latest_config_update)/60;
}

bool configIsTooOld() 
{
  return minutesSinceConfigUpdate() > 60;
}

void loop() 
{
  setGreen(HIGH);
  timeClient.update();

  int minutes = timeClient.getMinutes();
  if (minutes != minutes_previous) 
  {
    setGreen(LOW);
    minutes_previous = minutes;
    int hours = timeClient.getHours();
    
    Serial.print("Time is: ");
    Serial.println(timeClient.getFormattedTime());

    // update config with configured interval
    if (minutes % config.poll_interval_minutes == 0 || configIsTooOld())
    {
      // get config
      if (getGoogleConfig(configTmp))
      {
        unix_latest_config_update = timeClient.getEpochTime();
        if (!config.equals(configTmp))
        {
          config = configTmp;
          ifttt_webhook("Config update", true, config.formatted());
        }
      }
      else {
        // ifttt_webhook("Config update", false, getConfigError().c_str());
      }
    }

    if (config.force_open) 
    {
      doorOpen();
    }
    else if (config.force_close) 
    {
      doorClose();
    }
    
  
    if (config.open_hour == hours && (config.open_minutes == minutes || config.open_minutes+1 == minutes)) 
    {
      doorOpen();
    }
    else if (config.close_hour == hours && (config.close_minutes == minutes || config.close_minutes+1 == minutes)) 
    {
      doorClose();
    }

    // log status every X hours
    if (hours%1 == 0 && minutes%5 == 0) 
    {
      char buf[30];
      sprintf(buf, "%s: config age: %d minutes", timeClient.getFormattedTime().c_str(), minutesSinceConfigUpdate());
      ifttt_webhook("Board status", true, buf);
    }
  }

  setGreen(HIGH);
  if (doorButtonPressed())
  {
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
    delay(400);
  }

  delay(150);
}
