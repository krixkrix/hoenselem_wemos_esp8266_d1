#include <ESP8266HTTPClient.h>

// config is stored in a public google spreadsheet
// note the URL contains the doc ID and the modifier "format=csv"
const char* GetURL = "https://docs.google.com/spreadsheets/d/1mWT1SBtN5EKl85kzLBuUofBARWZpKznMYA6NtNMP_4Q/export?gid=0&format=csv";
// const char* DocumentId = "1mWT1SBtN5EKl85kzLBuUofBARWZpKznMYA6NtNMP_4Q";

class Config
{
  public:
    Config();
    bool sync();

  public:
    int open_hour = 07;
    int open_minutes = 10;
    int close_hour = 19;
    int close_minutes = 30;

  private:
    HTTPClient http;
};

Config::Config()
{

}

bool Config::sync()
{
  http.begin(GetURL);
  int httpCode = http.GET();
  if (httpCode > 0) { //Check the returning code
    String payload = http.getString();
    
    // TODO parse response
    // expected a key/value pair list like
    /*
    open_hour,6
    open_minutes,59
    close_hour,20
    close_minutes,30
    */
    Serial.println(payload);
  }
  http.end();
  return true;
}
