// PINs
const int RedLED = D3;  // D8 is best used for output, since it is pulled to ground
const int GreenLED = D8;


void setupLEDs()
{
  pinMode(LED_BUILTIN, OUTPUT);  // build-in LED == D4
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(GreenLED, OUTPUT);
  digitalWrite(GreenLED, LOW);
  pinMode(RedLED, OUTPUT);
  digitalWrite(RedLED, LOW);
}

void toggleRed()
{
  digitalWrite(RedLED, !digitalRead(RedLED));
}

void toggleGreen() 
{
  digitalWrite(GreenLED, !digitalRead(GreenLED));
}

void toggleRedAndGreen() 
{
  digitalWrite(RedLED, !digitalRead(GreenLED));
  digitalWrite(GreenLED, !digitalRead(GreenLED));
}

void setGreen(int on)
{
    digitalWrite(GreenLED, on);
}

void setRed(int on)
{
    digitalWrite(RedLED, on);
}
