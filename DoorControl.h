#ifndef DOOR_CONTROL_H
#define DOOR_CONTROL_H


// PINs motor
const int ControlPin1 = D5;  // cannot use D1, why?
const int ControlPin2 = D6;
const int EnablePin = D2;

// PINs motor inputs
const int ButtonPin = D10;
const int LimitBotPin = D4;
const int LimitTopPin = D7;

// door states

bool isMoving = false;
int latestDirection = -1;  // set latest direction down, so the first button-initiated move will be up

const int DoorTimeoutMs = 60000;
const int Speed = 1023;  // MaxSpeed 1023 is around 4sec/rev

// door input resolution (limit switches, buttons, LEDs...)
const int LoopSleep = 10;


void startMove(int direction) 
{
  digitalWrite(ControlPin1, direction == 1);
  digitalWrite(ControlPin2, direction == -1);
  analogWrite(EnablePin, Speed);
  //digitalWrite(EnablePin, 1);
  isMoving = true;
  latestDirection = direction;
}

void stopMove()
{
  analogWrite(EnablePin, 0);
  //digitalWrite(EnablePin, 0);
  isMoving = false;
}

bool doorIsMoving() 
{
  return isMoving;
}

int getDoorLatestDirection(){
  return latestDirection;
}

bool doorIsClosed()
{
  return digitalRead(LimitBotPin) == 1;
}
bool doorIsOpen()
{
  return digitalRead(LimitTopPin) == 1;
}


void doorStateInit() 
{
  pinMode(LimitTopPin, INPUT);
  pinMode(LimitBotPin, INPUT);
  pinMode(ControlPin1, OUTPUT);
  pinMode(ControlPin2, OUTPUT);
  pinMode(EnablePin, OUTPUT);
  latestDirection = 1;
  stopMove();
}

bool doorButtonPressed()
{
  return digitalRead(ButtonPin) == 1;
}

bool doorOpen() 
{
  if (doorIsOpen())
  {
    Serial.println(F("already open"));
    return true;
  }
  Serial.println(F("opening door"));
  setRed(LOW);
  startMove(1);
  for (int dt = 0; dt < DoorTimeoutMs; dt+=LoopSleep) 
  {
    if (dt%500 == 0) 
    {
      Serial.print("o"); 
      toggleGreen();
    }
    if (doorIsOpen())
    {
      break;
    }
    if (doorButtonPressed() && dt > 500) 
    {
      // cancel the move
      Serial.println(F("Cancel open"));
      break;
    }
    delay(LoopSleep);
  }
  stopMove();
  setGreen(HIGH);

  if (!doorIsOpen())
  {
    setGreen(LOW);
    report_door_open(false, F("ERROR: open switch NOT active"));
    return false;
  }
  if (doorIsClosed())
  {
    report_door_open(false, F("ERROR: closed switch active"));
    return false;
  }
  
  report_door_open(true, F("Door now open"));
  delay(500);
  return true;
}

bool doorClose() 
{
  if (doorIsClosed())
  {
    Serial.println(F("already closed"));
    return true;
  }
  
  Serial.println(F("closing door"));
  setGreen(LOW);
  startMove(-1);
  for (int dt = 0; dt < DoorTimeoutMs; dt+=LoopSleep) 
  {
    if (dt%500 == 0) 
    {
      Serial.print("c");
      toggleRed();
    }
    if (doorIsClosed())
    {
      break;
    }
    if (doorButtonPressed() && dt > 500)
    {
      // cancel the move
      Serial.println(F("Cancel close"));
      break;
    }
    delay(LoopSleep);
  }
  stopMove();
  setRed(HIGH);

  if (!doorIsClosed())
  {
    setRed(LOW);
    report_door_closed(false, F("ERROR: closed switch NOT active"));
    return false;
  }
  if (doorIsOpen())
  {
    report_door_closed(false, F("ERROR: open switch active"));
    return false;
  }
  
  report_door_closed(true, F("Door now closed"));
  delay(500);
  return true;
}


#endif
