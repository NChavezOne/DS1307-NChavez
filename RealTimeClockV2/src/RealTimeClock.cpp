/*
    RealTimeClock.h - A library designed for communication with the DS1307 real time clock IC.
    Made by Nicholas Chavez. Version 1: 10/10/19
    See library documentation for more information, and usage
*/

#include <Arduino.h>
#include <Wire.h>
#include "RealTimeClock.h"

RealTimeClock::RealTimeClock(void) {
  //do nothing
}


int RealTimeClock::begin(void) {
  Wire.begin(); //begin i2c communication as a master device
  Wire.beginTransmission(_addr);
  unsigned char ctrl_stat = Wire.endTransmission();
  return ctrl_stat;
}

void RealTimeClock::writeTime(int hour, int min, int sec) {
  //bailout from this function if the values are invalid
  if (hour >= 13 || min >= 60 || sec >= 60) {
    return;
  }

  uint8_t _hr, _tenhr = (hour / 10) << 4;
  _hr = hour % 10;
  _hr |= _tenhr;
  //we have to take into account that the hours register is shared w/ control bits
  _hr |= (readAddr(0x02) & 0b01100000);
  writeAddr(0x02,_hr);
  //same concept for minutes
  uint8_t _min, _tenmin = (min / 10) << 4;
  _min = min % 10;
  _min |= _tenmin;
  writeAddr(0x01,_min);
  //...and the same for seconds
  uint8_t _sec, _tensec = (sec / 10) << 4;
  _sec = sec % 10;
  _sec |= _tensec;
  writeAddr(0x00,_sec);
}

void RealTimeClock::toggleClock(void) {
  uint8_t val;
  val = ~((readAddr(0x00) & 0b10000000) >> 7);
  val &= readAddr(0x00);
  writeAddr(0x00,val);
  /*
  esentially toggles the CH bit (bit 7, MSB) of register 0x00
  which toggles the clock on.  
  */
}

int RealTimeClock::getSeconds(void) {
  uint8_t sec = readAddr(0x00);
  uint8_t tenSec = (sec & 0b01110000) >> 4; //masking off other bits, taking ten seconds and bitshifting
  sec &= 0b00001111; //seconds 
  return (tenSec * 10) + sec;
}

int RealTimeClock::getMinutes(void) {
  uint8_t min = readAddr(0x01);
  uint8_t tenMin = (min & 0b01110000) >> 4; 
  min &= 0b00001111;
  return (tenMin * 10) + min;
}

int RealTimeClock::getHours(void) {
  uint8_t mask = 0b00110000, tenHour, hr = readAddr(0x02); //by default the mask doesn't ignore bit 5
  unsigned char modeTest = (readAddr(0x02) & 0b01000000) >> 6; //defining bit six of address 0x02
  if (modeTest == 1) mask = 0b00010000; //if bit 6 is logic high, aka we're in 12 hr mode, ignore bit 5
  tenHour = (hr & mask) >> 4;
  hr &= 0b00001111;
  return (tenHour * 10) + hr;
}

int RealTimeClock::getAmpm(void) { //returns 1 if PM, 0 if AM
  unsigned char modeTest = (readAddr(0x02) & 0b00100000) >> 5; 
  return modeTest;
}
void RealTimeClock::setAmpm(unsigned int mod) { //1 for PM, 0 AM
  if (mod > 1) {
    return;
  }
  //make sure we dont affect the other bits already there
  uint8_t _mode = mod << 5;
  _mode |= readAddr(0x02);
  writeAddr(0x02,_mode);
}

void RealTimeClock::setMode(int mod) {
  switch(mod) {
    case 12:
      writeAddr(0x02,0b01000000);
      break;
    case 24:
      writeAddr(0x02,0b00000000);
      break;
    default:
      break;
  }
}

int RealTimeClock::getMode(void) {
  uint8_t mod = (readAddr(0x02) & 0b01000000) >> 6;
  switch (mod) {
    case 1:
      return 12;
    case 0:
      return 24;
    default:
      //should never happen?
      return 0;
  }
}

int RealTimeClock::readAddr(int loc) {
  Wire.beginTransmission(_addr);
  Wire.write(loc);
  Wire.endTransmission();
  Wire.requestFrom(_addr, 1);
  return (Wire.read());
}

void RealTimeClock::writeAddr(int loc, int val) {
  Wire.beginTransmission(_addr);
  Wire.write(loc);
  Wire.write(val);
  Wire.endTransmission();
}