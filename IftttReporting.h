#ifndef IFTTT_REPORTING_H
#define IFTTT_REPORTING_H

#include "DataMaker.h"
#include "WifiUtil.h"

// curl test expression
// curl --header "Content-Type: application/json" --request POST --data '{"value1":"xyz","value2":"another value"}' http://maker.ifttt.com/trigger/chickendoor/with/key/bl6Mm_2AoLXuaTuRyFJlrR

// declare new maker event with (maker key, event name)
DataToMaker event("bl6Mm_2AoLXuaTuRyFJlrR", "chickendoor");
DataToMaker error_event("bl6Mm_2AoLXuaTuRyFJlrR", "chickendoor_error");


void ifttt_webhook(const char* eventname, bool success, const char* msg)
{
  connectWifi();
  Serial.print(eventname);
  Serial.print(": ");
  Serial.print(success ? "ok" : "failure");
  Serial.print(": ");
  Serial.println(msg);

  // log message
  event.setValue(1, eventname);
  event.setValue(2, success ? "ok" : "failure");
  event.setValue(3, msg);
  if (event.connect())
  {
    event.post();
  }
  else 
  {
    Serial.println("Failed To Connect To Maker!");
  }

  // special notification when error
  if (!success)
  {
    error_event.setValue(1, eventname);
    error_event.setValue(2, "Failure");
    error_event.setValue(3, msg);
    if (error_event.connect())
    {
      error_event.post();
    }
    else 
    {
      Serial.println("ErrorEvent: Failed To Connect To Maker!");
    }
  }
}

void report_door_closed(bool ok, const char* msg)
{
  ifttt_webhook("Door close", ok, msg);
}

void report_door_open(bool ok, const char* msg)
{
  ifttt_webhook("Door open", ok, msg);
}

#endif
