// TLC5917IN LED Driver Library
//     by Ben Wang, 10/5/14
//     EECS 473 - University of Michigan - Ann Arbor

#include "LedDriver.h"
#include <Arduino.h>
#include <SPI.h>

void LedDriver::init(int latchPin, int enablePin) {
  // Initialize pins
  this->latchPin = latchPin;
  this->enablePin = enablePin;

  pinMode(latchPin, OUTPUT);
  pinMode(enablePin, OUTPUT);

  // Initial conditions
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
}

void LedDriver::write(char data) {
  // Write LED data bits
  SPI.transfer(data);
  
  // Latch data
  digitalWrite(latchPin, HIGH);
  digitalWrite(latchPin, LOW);
}

void LedDriver::setBrightness(int brightness) {
  analogWrite(enablePin, 255 - brightness);  // "255 - " Because active LOW
}

void LedDriver::enableOutput() {
  digitalWrite(enablePin, LOW);
}

void LedDriver::disableOutput() {
  digitalWrite(enablePin, HIGH);
}
