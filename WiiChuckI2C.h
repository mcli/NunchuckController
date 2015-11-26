/*
  WiiChuckI2C Library for Wii Nunchuck 
  Copyright (c) 2015 Maurice Ling.  All rights reserved.
*/

#if(ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>

#ifndef WII_CHUCK_H
#define WII_CHUCK_H

enum WiiChuckButton
{
  BUTTON_NONE,
  BUTTON_C,
  BUTTON_Z
};

#define NUNCHUCK_ADDRESS 0x52

/**
 * For Micromagician:
 * Connect SDA to Pin A4, SCL to Pin A5
 * References: 
 * http://www.instructables.com/id/Step-by-Step-guide-to-Micro-Magician-robot-control/step8/Analog-inputs/
 * http://dsscircuits.com/articles/86-articles/66-arduino-i2c-master-library
 **/
class WiiChuckI2C
{
public:
  WiiChuckI2C();

  /**
   * initialize the nunchuck
   * @return 0 if success, 1 if error
   */
  int init();

  /**
   * Read the nunchuck values
   * @param joy Array of two integers to hold joystick x,y position info.
   * @param accel Array of 3 integers to hold accelerometer info
   * @return 0 if success, 1 if error
   **/
  int read(int *joy, int *accel, WiiChuckButton &button);
  
};

extern WiiChuckI2C  wiiChuckI2C;

#endif
