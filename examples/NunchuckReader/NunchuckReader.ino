/*
 * Example program using NunchuckController
 */

#include <NunchuckController.h>
#include <I2C.h>

void setup()
{
  Serial.begin(115200);  // Initialize serial port for debugging

  int status=nunchuckCtl.init(); // initialize nunchuck
  if (0 != status)
  {
    Serial.println("Error initializing nunchuckCtl");
  }
//  int sensitivity[3]={10,100,100};
//  nunchuckCtl.setAccelSensitivity(sensitivity);
}

void loop()
{
  static int count=0;
  static int lastStatus=-1;

  static NunchuckButton lastButton=BUTTON_NONE;
  NunchuckButton button;

  static int lastJoy[2]={0};
  
  int accel[3]={0};
  int joy[2]={0};

  // read the nunchuck data
  int readStatus=nunchuckCtl.read(joy, accel, button);

  bool printValues=false;

  for (int i = 0; i < 2; i ++)
  {
    if (joy[i] != lastJoy[i])
    {
      lastJoy[i] = joy[i];
      printValues=true;
    }
  }
  if (button != lastButton)
  {
    lastButton=button;
    printValues=true;
  }

  for (int i = 0; i < 3; i++)
  {
    if (nunchuckCtl.deltaAccel_[i] != 0)
    {
      printValues=true;
    }
    
  }
  // print the data if retrieval was successful and if there is a change in
  // state (button pushed, joystick, or accel change)
  if (0==readStatus && printValues)            
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
      Serial.print("(");
      Serial.print(nunchuckCtl.deltaAccel_[i]);
      Serial.print(")");
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

