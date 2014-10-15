// TLC5917IN LED Driver Library
//     by Ben Wang, 10/5/14
//     EECS 473 - University of Michigan - Ann Arbor
#ifndef LED_DRIVER_H
#define LED_DRIVER_H

class LedDriver {

  int latchPin, enablePin;

  public:
    void init(int latchPin, int enablePin);
    void write(char data);
    void setBrightness(int brightness);
    void enableOutput();
    void disableOutput();
};

#endif
