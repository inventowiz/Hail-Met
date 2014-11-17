#include "HelmetIO.h"


// Bits that represent lights
// MSB
// 7 - Call Status
// 6 - Right Rear
// 5 - Right Middle
// 4 - Right Front
// 3 - Taillight
// 2 - Left Rear
// 1 - Left Middle
// 0 - Left Front

char HelmetIO::generateLedStates() {
  char res = 0;
  
  if (taillight) {
    res |= 0x8;
  }
  
  if (leftSignal && leftSignalsActive == 0) {
    res |= 0x7;
    leftSignalsActive = 0x7;
  } else {
    res &= ~(0x7);
    leftSignalsActive = 0x0;
  }
  
  if (rightSignal && rightSignalsActive == 0) {
    res |= 0x70;
    rightSignalsActive = 0x7;
  } else {
    res &= ~(0x70);
    rightSignalsActive = 0x0;
  }
  
  if (callStatus != NO_CALL || lowBattery) {
    if (callStatus == INCOMING_CALL) {
      if (!indicatorActive) {
        res |= 0x80;
        indicatorActive = true;
      } else {
        res &= ~(0x80);
        indicatorActive = false;
      }
    } else if (callStatus == ACTIVE_CALL) {
      res |= 0x80;
      indicatorActive = true;
    } else if (lowBattery) {
      if (lowBatteryDelay == 0) {
        res |= 0x80;
        indicatorActive = true;
      } else {
        res &= ~(0x80);
        indicatorActive = false;
      }
      
      if (lowBatteryDelay >= 3) {
        lowBatteryDelay = 0;
      } else {
        lowBatteryDelay++;
      }
    }
  } else {
    res &= ~(0x80);
    indicatorActive = true;
  }
  
  return res;
}

void HelmetIO::init(LedDriver &ledDriver) {
  this->ledDriver = &ledDriver;
  leftSignalsActive = 0;
  rightSignalsActive = 0;
  indicatorActive = false;
  headlightBrightness = 255;
  lowBatteryDelay = 0;
  callStatus = NO_CALL;
  
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
  taillight = false;
}

void HelmetIO::enableLeftTurnSignal() {
  leftSignal = true;
}

void HelmetIO::disableLeftTurnSignal() {
  leftSignalsActive = 0;
  leftSignal = false;
}

bool HelmetIO::getLeftTurnSignal() {
  return leftSignal;
}

void HelmetIO::enableRightTurnSignal() {
  rightSignal = true;
}

void HelmetIO::disableRightTurnSignal() {
  rightSignalsActive = 0;
  rightSignal = false;
}

bool HelmetIO::getRightTurnSignal() {
  return rightSignal;
}

void HelmetIO::setCallStatus(int status) {
  callStatus = status;
  if (callStatus == NO_CALL) {
    indicatorActive = false;
  }
}

void HelmetIO::enableLowBattery() {
  lowBattery = true;
}

void HelmetIO::disableLowBattery() {
  lowBattery = false;
}

