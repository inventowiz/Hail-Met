#ifndef HELMET_IO_H
#define HELMET_IO_H

#include "LedDriver.h"
#include "Arduino.h"

#define PIN_HEADLIGHT 5

class HelmetIO {
  private:
    LedDriver *ledDriver;
    bool headlight, taillight, leftSignal, rightSignal;
    int headlightBrightness;  // [0,255], Represents brightness of headlight (pwm value)
    char leftSignalsActive, rightSignalsActive;
   
    char generateLedStates();
    
  public:
  
    void init(LedDriver &ledDriver);
    void updateLights();
    void enableHeadlight();              // Turns headlight on with specified brightness
    void disableHeadlight();             // Turns headlight off, but saves brightness
    void setHeadlightBrightness(int brightness);  // Sets PWM value of headlight. Call enableHeadlight() to turn on.
    void enableTaillight();
    void disableTaillight();
    void enableLeftTurnSignal();
    void disableLeftTurnSignal();
    bool getLeftTurnSignal();
    void enableRightTurnSignal();
    void disableRightTurnSignal();
    bool getRightTurnSignal();
};

#endif
