#include "HelmetIO.h"


// Bits that represent lights
// MSB
// 7 - Right Front
// 6 - Right Middle
// 5 - Right Rear
// 4 - Left Front
// 3 - Left Middle
// 2 - Left Rear
// 1 - Headlight
// 0 - Taillight

char HelmetIO::generateLedStates() {
  char res = 0;
  
  if (taillight) {
    res = 0x1;
  }
  
  if (headlight) {
    res |= 0x2;
  }
  
  if (leftSignal && leftSignalsActive == 0) {
    res |= 0x1C;
    leftSignalsActive = 0x7;
  } else {
    res &= ~(0x1C);
    leftSignalsActive = 0x0;
  }
  
  if (rightSignal && rightSignalsActive == 0) {
    res |= 0xE0;
    rightSignalsActive = 0x7;
  } else {
    res &= ~(0xE0);
    rightSignalsActive = 0x0;
  }
  
  return res;
}

void HelmetIO::init(LedDriver &ledDriver) {
  this->ledDriver = &ledDriver;
  leftSignalsActive = 0;
  rightSignalsActive = 0;
  
  this->ledDriver->write(0);
}

// Called within loop (or timer interrupt)
void HelmetIO::updateLights() {
  ledDriver->write(generateLedStates());
}

void HelmetIO::enableHeadlight() {
  headlight = true;
  analogWrite(PIN_HEADLIGHT, headlightBrightness);
}

void HelmetIO::disableHeadlight() {
  headlight = false;
  digitalWrite(PIN_HEADLIGHT, LOW);
}

void HelmetIO::setHeadlightBrightness(int brightness){
  headlightBrightness = brightness;
}

void HelmetIO::enableTaillight() {
  taillight = true;
}

void HelmetIO::disableTaillight() {
  taillight = true;
}

void HelmetIO::enableLeftTurnSignal() {
  leftSignal = true;
}

void HelmetIO::disableLeftTurnSignal() {
  leftSignalsActive = 0;
  leftSignal = false;
}

void HelmetIO::enableRightTurnSignal() {
  rightSignal = true;
}

void HelmetIO::disableRightTurnSignal() {
  rightSignalsActive = 0;
  rightSignal = false;
}
