#ifndef GOOGLE_CONFIG_H
#define GOOGLE_CONFIG_H

#include <ESP8266HTTPClient.h>
#include <StackThunk.h>

// config is stored in a public google spreadsheet
// note the URL contains the doc ID , the modifier "format=csv", and a specific cell range

const char* host = "docs.google.com";
const int httpsPort = 443;
const String link = "/spreadsheets/d/1mWT1SBtN5EKl85kzLBuUofBARWZpKznMYA6NtNMP_4Q/export?gid=0&format=csv&range=A3:B9";  // The RANGE here is crucial
String latestError = "Not initialized";

class Config {

public:
  int open_hour = 7;
  int open_minutes = 15;
  int close_hour = 19;
  int close_minutes = 30;
  int poll_interval_minutes = 10;
  int force_open = 0;
  int force_close = 0;

  const char* formatted() 
  {
    static char buf[40];
    sprintf(buf, "Open: %02d:%02d, Close: %02d:%02d, Poll: %d", open_hour, open_minutes, close_hour, close_minutes, poll_interval_minutes);
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

const String& getConfigError()
{
  return latestError;  
}

bool getGoogleConfig(Config& config)
{
  latestError = "";
  HTTPClient https;
  BearSSL::WiFiClientSecure newSecure;

  // do not use fingerprint since it will change over time
  Serial.println("HTTPS connecting (insecure)");
  newSecure.setInsecure();
  https.begin(newSecure, host, httpsPort, link.c_str(), true);

  int code = https.GET();
  if (code != 200)
  {
    https.end();
    newSecure.stop();
    Serial.print("Https failed, code: ");
    Serial.println(code);
    latestError = "connect failure";
    return false;
  }
  
  // example reply
    /*
    open_hour,23
    open_minutes,59
    close_hour,12
    close_minutes,13
    ...
    */
  String payload = https.getString();
  Serial.println(payload);

  // debug ...
  Serial.printf("[%d] ", stack_thunk_get_max_usage());
  
  https.end();
  newSecure.stop();

  int n = sscanf(payload.c_str(), 
                 "open_hour,%d open_minutes,%d close_hour,%d close_minutes,%d poll_interval_minutes,%d force_open,%d force_close,%d", 
                 &config.open_hour,
                 &config.open_minutes, 
                 &config.close_hour,
                 &config.close_minutes,
                 &config.poll_interval_minutes,
                 &config.force_open,
                 &config.force_close);           
  
  Serial.print("Got paramters: ");
  Serial.println(n);

  if (n!=7) 
  {
    latestError = "Parse failure";
    return false;
  }

  // don't accept any strange data
  if (config.poll_interval_minutes < 1
      || config.open_hour < 1 
      || config.open_hour > 24
      || config.close_hour < 1
      || config.close_hour > 24
      || config.open_minutes < 0 
      || config.open_minutes > 59
      || config.close_minutes < 0
      || config.close_minutes > 59
      || n != 7)
  {
    latestError = "Bad content";
    return false;
  }
  return true;  // success
}


#endif
