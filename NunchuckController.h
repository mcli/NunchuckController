/*
  Nunchuck Controller Library for Wii Nunchuck 
  Copyright (c) 2015 Maurice Ling.  All rights reserved.
*/

#if(ARDUINO >= 100)
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

#include <inttypes.h>

#ifndef NUNCHUCK_CONTROLLER_H
#define NUNCHUCK_CONTROLLER_H

enum NunchuckButton
{
  BUTTON_NONE,
  BUTTON_C,
  BUTTON_Z
};

#define NUNCHUCK_ADDRESS 0x52

/**
 * Nunchuck utility library.
 *
 * For Micromagician, Connect SDA to Pin A4, SCL to Pin A5
 *
 * References:
 * 
 * Micromagician:  
 *  - http://www.instructables.com/id/Step-by-Step-guide-to-Micro-Magician-robot-control/step8/Analog-inputs/
 * 
 * i2c-master-library:
 *  - http://dsscircuits.com/articles/86-articles/66-arduino-i2c-master-library
 *
 * nunchuck i2c interface:
 *  - http://web.engr.oregonstate.edu/~johnstay/ece375/pdf/nunchuk.pdf
 *  - http://www.robotshop.com/media/files/PDF/inex-zx-nunchuck-datasheet.pfd
 **/
class NunchuckController
{
public:
  NunchuckController();

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
  int read(int *joy, int *accel, NunchuckButton &button);

  /**
   * Set the joystick center value to be the current value.  Make sure
   * Joy stick is in center position.
   * @return 0 if successful
   * @return 1 if error
   **/
  int calibrateJoyCenter(void);

  /**
   * Set accelerometer change sensitivity.
   **/
  void setAccelSensitivity(int *sensitivity);
  
  /**
   * Delta accelerometer readings
   **/
  int deltaAccel_[3];
  
private:
  uint8_t joyCenter_[2];
  int lastAccel_[3];
  int accelSensitivity_[3];
};

extern NunchuckController  nunchuckCtl;

#endif
