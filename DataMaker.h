#ifndef IFTTT_DATAMAKER_H
#define IFTTT_DATAMAKER_H

/**
 * Class to build and send a HTTP post request to ifttt webhook
 * copied from
 * https://github.com/mylob/ESP-To-IFTTT/blob/master/ESP8266_To_IFTTT/DataToMaker.h
 */


#include <ESP8266WiFi.h>

class DataToMaker
{
  public:
    DataToMaker(const char*); // constructor
    bool connect();
    bool setValue(int, String);
    void sendToMaker();
    void post(String);

  protected: // it is protected because the subclass needs access
    //to max distance!

  private:
    void compileData();
    WiFiClient client;
    const char* privateKey;
    String value1, value2, value3 = "";
    bool dataAvailable;
    String postData;
};

DataToMaker::DataToMaker(const char* _privateKey)
{
  privateKey = _privateKey;
}

bool DataToMaker::connect()
{
  int r = 50;  // retries
  while ((!client.connect("maker.ifttt.com", 80)) && (r > 0)){
      delay(100);
      Serial.print(".");
      r--;
  }
  return r>0;
}

void DataToMaker::post(String event)
{
  compileData();
  client.print("POST /trigger/");
  client.print(event);
  client.print("/with/key/");
  client.print(privateKey);
  client.println(" HTTP/1.1");

  client.println("Host: maker.ifttt.com");
  client.println("User-Agent: Arduino/1.0");
  client.println("Connection: close");
  if (dataAvailable)
  { // append json values if available
    client.println("Content-Type: application/json");
    client.print("Content-Length: ");
    client.println(postData.length());
    client.println();
    client.println(postData);
  }
  else
    client.println();
}

bool DataToMaker::setValue(int valueToSet, String value)
{
  switch (valueToSet)
  {
    case 1:
      value1 = value;
      break;
    case 2:
      value2 = value;
      break;
    case 3:
      value3 = value;
      break;
    default:
      return false;
      break;
  }
  return true;
}

void DataToMaker::compileData()
{
  if (value1 != "" || value2 != "" || value3 != "")
  {
    dataAvailable = true;
    bool valueEntered = false;
    postData = "{";
    if (value1 != "")
    {
      postData.concat("\"value1\":\"");
      postData.concat(value1);
      valueEntered = true;
    }
    if (value2 != "")
    {
      if (valueEntered)postData.concat("\",");
      postData.concat("\"value2\":\"");
      postData.concat(value2);
      valueEntered = true;
    }
    if (value3 != "")
    {
      if (valueEntered)postData.concat("\",");
      postData.concat("\"value3\":\"");
      postData.concat(value3);
    }
    postData.concat("\"}");
  }
  else dataAvailable = false;
}

#endif
