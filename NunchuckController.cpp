#include <I2C.h>
#include "NunchuckController.h"

/**
 * Buffer to store nunchuck data
 **/
static unsigned char nunchuckBuf[6];

NunchuckController::NunchuckController()
{
  joyCenter_[0]=114;
  joyCenter_[1]=114;
  deltaAccel_[0]=lastAccel_[0]=0;
  deltaAccel_[1]=lastAccel_[1]=0;
  deltaAccel_[2]=lastAccel_[2]=0;

  int sensitivity[3]={10,10,10};
  setAccelSensitivity(sensitivity);
}

/**
 * Initialize the wiiChuck
 **/
int NunchuckController::init()
{
  int pingResult=I2c.ping(NUNCHUCK_ADDRESS);
  switch (pingResult)
  {
  case 0:
    Serial.print("Nunchuck not responding at ");
    Serial.println(NUNCHUCK_ADDRESS, HEX);
    break;
  case 1:
    Serial.println("Nunchuck is alive!");
    break;
  case -1:
    Serial.println("Communications problem");
    break;
  }

  I2c.begin();
  int status=I2c.write(NUNCHUCK_ADDRESS,0x40,0x00); // talk to nunchuck
  delay(100); // pause for things to settle down
  return status;
}

/**
 * buf[0] - joy X axis
 * buf[1] - joy Y axis
 * buf[2] - accel x MSB
 * buf[3] - accel y MSB
 * buf[4] = accel z MSB
 * accelerometer data is 10 bits
 * 
 * nunchuckBuf[5] bits:
 *  0 - z Button
 *  1 - c Button
 *  2, 3 - LSB X
 *  4, 5 - LSB Y
 *  6, 7 - LSB Z
 **/
int NunchuckController::read(int *joy, int *accel, NunchuckButton &button)
{
  int writeStatus=I2c.write(NUNCHUCK_ADDRESS,0x00); // Conversion command
  if (0 != writeStatus)
  {
    return writeStatus;
  }
  delay(100);
  int readStatus=I2c.read(NUNCHUCK_ADDRESS,6,nunchuckBuf);

  //
  // Decode values for accelerometer, skipping joystick values which are not
  // encoded.
  //
  for (int i = 2; i < sizeof(nunchuckBuf); i++)
  {
    nunchuckBuf[i]=(nunchuckBuf[i]^0x17)+0x17;
  }

  // convert raw values into normalized values
  joy[0]=(int)nunchuckBuf[0]-joyCenter_[0];
  joy[1]=(int)nunchuckBuf[1]-joyCenter_[1];
  accel[0]=(nunchuckBuf[2]<<2)+((nunchuckBuf[5]>>2)&0x3)-accelOffset_[0];
  accel[1]=(nunchuckBuf[3]<<2)+((nunchuckBuf[5]>>4)&0x3)-accelOffset_[1];
  accel[2]=(nunchuckBuf[4]<<2)+((nunchuckBuf[5]>>6)&0x3)-accelOffset_[2];

  for (int i = 0; i < 3; i++)
  {
    if (lastAccel_[i] != 0)
    {
      int delta=accel[i]-lastAccel_[i];
      if (abs(delta) > accelSensitivity_[i])
      {
        deltaAccel_[i]=delta;
      } else
      {
        deltaAccel_[i]=0;
      }
    }
    lastAccel_[i]=accel[i];
  }
  
  // invert the button values (raw 1 is not pressed)
  if (!(nunchuckBuf[5]&1))
  {
    button=BUTTON_Z;
  } else if (((nunchuckBuf[5]>>1)&1) == 0)
  {
    button=BUTTON_C;
  } else
  {
    button=BUTTON_NONE;
  }
  return readStatus;
}

float NunchuckController::computeRoll(int *accel)
{
  return atan(accel[0]/sqrt(pow(accel[1],2)+pow(accel[2],2)));
}

float NunchuckController::computePitch(int *accel)
{
  return -1*atan(accel[1]/sqrt(pow(accel[0],2)+pow(accel[2],2)));
}

int NunchuckController::calibrateJoyCenter(void)
{
  int joyValue[2], accel[3];
  NunchuckButton button;
  int status = read(joyValue, accel, button);
  if (0 == status)
  {
    if (0 != joyValue[0])
    {
      joyCenter_[0]-=joyValue[0];
    }
    if (0 != joyValue[1])
    {
      joyCenter_[1]-= joyValue[1];
    }
  }
  Serial.print("Joystick Center:  (");
  Serial.print(joyCenter_[0]);
  Serial.print(", ");
  Serial.print(joyCenter_[0]);
  Serial.println(")");
}

/**
 * Calibrate x and y zeros, then use the average to estimate the z zero
 **/
int NunchuckController::calibrateAccel(void)
{
  int joyValue[2], accel[3];
  NunchuckButton button;
  int status = read(joyValue, accel, button);

  accelOffset_[0]=accel[0];
  accelOffset_[1]=accel[1];
  accelOffset_[2]=(accel[0]+accel[1])/2;

  Serial.print("Accel Offsets:  (");
  for (int i = 0; i < 2; i++)
  {
    if (i != 0)
    {
      Serial.print(", ");
    }
    Serial.print(accel[i]);
  }
  Serial.println(")");
  return status;
}

void NunchuckController::setAccelSensitivity(int *sensitivity)
{
  for (int i = 0; i < 3; i++)
  {
    accelSensitivity_[i]=sensitivity[i];
  }
}

NunchuckController nunchuckCtl=NunchuckController();
