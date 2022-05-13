/*
    RealTimeClock.h - A library designed for communication with the DS1307 real time clock IC.
    Made by Nicholas Chavez. Version 1: 10/10/19
    See library documentation for more information, and usage
*/
#ifndef RealTimeClock_h
#define RealTimeClock_h

#include <Arduino.h>

class RealTimeClock
{
public:
  RealTimeClock();

  int begin(void);
  void toggleClock(void);
  
  void setMode(int mod);
  int getMode(void);

  void writeTime(int hr, int min, int sec);
  int getSeconds(void);
  int getMinutes(void);
  int getHours(void);

  int getAmpm(void);
  void setAmpm(unsigned int mod);
  
private:
  int8_t _addr = 0x68; //global i2c address for the DS1307
  void writeAddr(int loc, int val);
  int readAddr(int loc);
};

#endif