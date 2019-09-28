
// curl test expression
// curl --header "Content-Type: application/json" --request POST --data '{"value1":"xyz","value2":"another value"}' http://maker.ifttt.com/trigger/chickendoor/with/key/bl6Mm_2AoLXuaTuRyFJlrR

// declare new maker event with (maker key, event name)
DataToMaker event("bl6Mm_2AoLXuaTuRyFJlrR", "chickendoor");
// DataToMaker error_event("bl6Mm_2AoLXuaTuRyFJlrR", "chickendoor_error");


void ifttt_webhook(const char* eventname, const char* outcome, const char* msg)
{
  connectWifi();
  Serial.print(eventname);
  Serial.print(": ");
  Serial.print(outcome);
  Serial.print(": ");
  Serial.println(msg);
  
  event.setValue(1, eventname);
  event.setValue(2, outcome);
  event.setValue(3, msg);
  if (event.connect())
  {
    event.post();
  }
  else 
  {
    Serial.println("Failed To Connect To Maker!");
  }
}

void report_door_closed(bool ok, const char* msg)
{
  ifttt_webhook("Door close", ok ? "ok" : "failed", msg);
}

void report_door_open(bool ok, const char* msg)
{
  ifttt_webhook("Door open", ok ? "ok" : "failed", msg);
}
