#ifndef HELMET_IO_H
#define HELMET_IO_H

#include "LedDriver.h"
#include "Arduino.h"

#define NO_CALL 0
#define INCOMING_CALL 1
#define ACTIVE_CALL 2

class HelmetIO {
  private:
    LedDriver *ledDriver;
    bool taillight, leftSignal, rightSignal, headlight;
    bool lowBattery, indicatorActive, taillightActive;
    int headlightPin;
    int lowBatteryDelay;
    int callStatus;
    int headlightBrightness;  // [0,255], Represents brightness of headlight (pwm value)
    char leftSignalsActive, rightSignalsActive;
   
    char generateLedStates();
    
  public:
  
    void init(LedDriver &ledDriver, int headlightPin);
    void updateLights();
    void enableHeadlight();              // Turns headlight on with specified brightness
    void disableHeadlight();             // Turns headlight off, but saves brightness
    void setHeadlightBrightness(int brightness);  // Sets PWM value of headlight. Call enableHeadlight() to turn on.
    void updateHeadlightBrightness();
    bool getHeadlight();
    void enableTaillight();
    void disableTaillight();
    void enableLeftTurnSignal();
    void disableLeftTurnSignal();
    bool getLeftTurnSignal();
    void enableRightTurnSignal();
    void disableRightTurnSignal();
    bool getRightTurnSignal();
    void setCallStatus(int status);
    void enableLowBattery();
    void disableLowBattery();
};

#endif
