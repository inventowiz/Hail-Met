#ifndef HELMET_IO_H
#define HELMET_IO_H

#include "LedDriver.h"
#include "Arduino.h"

#define PIN_HEADLIGHT 5

class HelmetIO {
  private:
    LedDriver *ledDriver;
    bool headlight, taillight, leftSignal, rightSignal;
    char leftSignalsActive, rightSignalsActive;
   
    char generateLedStates();
    
    int headlightBrightness = 255;  // [0,255], Represents brightness of headlight (pwm value)
    
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
    void enableRightTurnSignal();
    void disableRightTurnSignal();
};

#endif
