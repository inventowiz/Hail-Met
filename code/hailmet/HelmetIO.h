#ifndef HELMET_IO_H
#define HELMET_IO_H

#include "LedDriver.h"

class HelmetIO {
  private:
    LedDriver *ledDriver;
    bool headlight, taillight, leftSignal, rightSignal;
    char leftSignalsActive, rightSignalsActive;
   
    char generateLedStates();
    
  public:
  
    void init(LedDriver &ledDriver);
    void updateLights();
    void enableHeadlight();
    void disableHeadlight();
    void enableTaillight();
    void disableTaillight();
    void enableLeftTurnSignal();
    void disableLeftTurnSignal();
    void enableRightTurnSignal();
    void disableRightTurnSignal();
};

#endif
