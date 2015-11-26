#include <I2C.h>
#include "WiiChuckI2C.h"

/**
 * Buffer to store nunchuck data
 **/
static unsigned char nunchuckBuf[6];

WiiChuckI2C::WiiChuckI2C()
{
}

/**
 * Initialize the wiiChuck
 **/
int WiiChuckI2C::init()
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
int WiiChuckI2C::read(int *joy, int *accel, WiiChuckButton &button)
{
  int writeStatus=I2c.write(NUNCHUCK_ADDRESS,0x00); // Conversion command
  if (0 != writeStatus)
  {
    return writeStatus;
  }
  delay(100);
  int readStatus=I2c.read(NUNCHUCK_ADDRESS,6,nunchuckBuf);

  // convert raw values into normalized values
  const int joyCenter=114;
  const int accelOffset[3]={0,0,0};
  joy[0]=(int)nunchuckBuf[0]-joyCenter;
  joy[1]=(int)nunchuckBuf[1]-joyCenter;
  accel[0]=(nunchuckBuf[2]<<2)+((nunchuckBuf[5]>>2)&0x3)-accelOffset[0];
  accel[1]=(nunchuckBuf[3]<<2)+((nunchuckBuf[5]>>4)&0x3)-accelOffset[1];
  accel[2]=(nunchuckBuf[4]<<2)+((nunchuckBuf[5]>>6)&0x3)-accelOffset[2];
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

WiiChuckI2C wiiChuckI2C=WiiChuckI2C();
