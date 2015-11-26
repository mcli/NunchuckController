/*
 * Example program using WiiChuckI2C
 */

#include <WiiChuckI2C.h>
#include <I2C.h>

void setup()
{
  Serial.begin(115200);  // Initialize serial port for debugging

  int status=wiiChuckI2C.init(); // initialize nunchuck
  if (0 != status)
  {
    Serial.println("Error initializing wiiChuckI2C");
  }
}

void loop()
{
  static int count=0;
  static int lastStatus=-1;

  WiiChuckButton button;
  
  int accel[3]={0};
  int joy[2]={0};

  // read the nunchuck data
  int readStatus=wiiChuckI2C.read(joy, accel, button);

  if (0==readStatus)            // print the data if retrieval was successful
  {
    Serial.print("Joy:  ");
    for (int i = 0; i < 2; i++)
    {
      Serial.print(joy[i], DEC);
      Serial.print(", ");
    }
    Serial.print("Accel:  ");
    for (int i = 0; i< 3; i++)
    {
      if (i) Serial.print(", ");
      Serial.print(accel[i], DEC);
    }
    switch(button)
    {
    case BUTTON_Z:
      Serial.print(", [Z Button]");
      break;
    case BUTTON_C:
      Serial.print(", [C Button]");
      break;
    }
    Serial.println();
    lastStatus=0;
  } else
  {
    if (lastStatus != readStatus)
    {
      Serial.print("Error Reading:  Status ");
      Serial.println(readStatus);
      lastStatus=readStatus;
    }
  }
}

