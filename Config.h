#ifndef GOOGLE_CONFIG_H
#define GOOGLE_CONFIG_H

#include <ESP8266HTTPClient.h>
#include "IftttReporting.h"

// config is stored in a public google spreadsheet
// note the URL contains the doc ID , the modifier "format=csv", and a specific cell range

const char* host = "docs.google.com";
const String link = "/spreadsheets/d/1mWT1SBtN5EKl85kzLBuUofBARWZpKznMYA6NtNMP_4Q/export?gid=0&format=csv&range=A3:B9";  // The RANGE here is crucial
const char fingerprint[] PROGMEM = "8D 80 59 8E C1 8A 28 15 C0 CF 83 80 00 4B F7 F2 86 F9 B1 1C";

const int httpsPort = 443;

class Config {

public:
  int open_hour = 8;
  int open_minutes = 00;
  int close_hour = 20;
  int close_minutes = 00;
  int poll_interval_minutes = 10;
  int force_open = 0;
  int force_close = 0;

  const char* formatted() 
  {
    static char buf[40];
    sprintf(buf, "Open: %02d:%02d, Close: %02d:%02d", open_hour, open_minutes, close_hour, close_minutes);
    return buf;
  }

  void print() {
    Serial.println(formatted());
  }

  bool equals(const Config& other)
  {
    return open_hour == other.open_hour
    && open_minutes == other.open_minutes
    && close_hour == other.close_hour
    && close_minutes == other.close_minutes
    && poll_interval_minutes == other.poll_interval_minutes
    && force_open == other.force_open
    && force_close == other.force_close;
  }
};


bool getGoogleConfig(Config& config)
{
  WiFiClientSecure httpsClient;

  httpsClient.setFingerprint(fingerprint);
  httpsClient.setTimeout(15000); // 15 Seconds

  Serial.println("HTTPS Connecting");
  int r=10; //retry counter
  while (r > 0){
     connectWifi();
     if (httpsClient.connect(host, httpsPort))
     {
       break;
     }
     delay(100);
     Serial.print(".");
     r--;
  }
  if (r==0) {
    connectWifi();
    ifttt_webhook("Google config read", false, "Connection failed");
    return false;
  }

  
  Serial.println("Connected to host");  
  // Serial.print("requesting URL: ");
  // Serial.println(host+link);
 
  httpsClient.print(String("GET ") + link + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +               
               "Connection: close\r\n\r\n");
 
  // Serial.println("request sent");
                  
  while (httpsClient.connected()) {
    String line = httpsClient.readStringUntil('\n');
    if (line == "\r") {
      // Serial.println("headers received");
      break;
    }
  }


  // example reply
  /*
   open_hour,23
   open_minutes,59
   close_hour,12
   close_minutes,13
  */

  int n = 0;

  // Serial.println("=== reply was: ===");
  String line;
  while(httpsClient.available()){   
    line = httpsClient.readStringUntil('\n');  // Read Line by Line
    // Serial.println(line); //Print response

    if (
      sscanf(line.c_str(), "open_minutes,%d", &config.open_minutes) > 0
      || sscanf(line.c_str(), "open_hour,%d", &config.open_hour) > 0
      || sscanf(line.c_str(), "close_minutes,%d", &config.close_minutes) > 0
      || sscanf(line.c_str(), "close_hour,%d", &config.close_hour) > 0
      || sscanf(line.c_str(), "poll_interval_minutes,%d", &config.poll_interval_minutes) > 0
      || sscanf(line.c_str(), "force_open,%d", &config.force_open) > 0
      || sscanf(line.c_str(), "force_close,%d", &config.force_close) > 0
        ) {
        n++;
    }
    else {
      Serial.println("Line was not recognized");
    }
 }

 // don't accept any errors
 if (config.poll_interval_minutes < 1
      || config.open_hour < 1 
      || config.open_hour > 24
      || config.close_hour < 1
      || config.close_hour > 24
      || config.open_minutes < 0 
      || config.open_minutes > 59
      || config.close_minutes < 0
      || config.close_minutes > 59
      || n < 7)
 {
   return false;
 }

 return true;
}


#endif
