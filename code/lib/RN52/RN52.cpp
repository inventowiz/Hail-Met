#include "RN52.h"
#include "Arduino.h"

void RN52::updateConnectionStatus() {
  int val;
  char inChar;

  serial->print("Q\r");
  String res = waitForResponse();

  prevState = state;

  // Most significant nibble of byte 0
  // Ignore

  // Least significant nibble of byte 0
  // Set profile connection statuses
  val = ASCIIToNum(res[1]);
  state.activeProfiles.iAP = val & 1;
  state.activeProfiles.SPP = (val >> 1) & 1;
  state.activeProfiles.A2DP = (val >> 2) & 1;
  state.activeProfiles.HFP_HSP = (val >> 3) & 1;

  // Most significant nibble of byte 1
  // Bits 6 - 7 (2 - 3 of nibble) are reserved/ignored
  // Set volume change statuses
  val = ASCIIToNum(res[2]);
  state.speakerVolumeChange = val & 1;
  state.microphoneVolumeChange = (val >> 1) & 1;

  // Least significant nibble of byte 1
  // Set connection status
  val = ASCIIToNum(res[3]);
  state.connectionStatus = (int) (val);
}

void RN52::init(Stream &serial, int gpio9Pin) {
  this->serial = &serial;
  this->gpio9Pin = gpio9Pin;
  this->isCommanding = false;

  pinMode(gpio9Pin, OUTPUT);

  enterCommandMode();
  updateConnectionStatus();
  exitCommandMode();
}

bool RN52::isInCommandMode() {
  return isCommanding;
}

bool RN52::enterCommandMode() {
  digitalWrite(gpio9Pin, LOW);
  String res = waitForResponse();

  isCommanding = res == "CMD\r";

  return isInCommandMode();
}

bool RN52::exitCommandMode() {
  digitalWrite(gpio9Pin, HIGH);
  String res = waitForResponse();

  isCommanding = !(res == "END\r");

  return !(isInCommandMode());
}

String RN52::waitForSerial(int length) {
  String ret = "";
  int waitLength = length;

  return ret;
}

String RN52::waitForResponse() {
  return serial->readStringUntil(terminatingChar);
}

bool RN52::isDiscoverable() {
  return state.connectionStatus == 2;
}

void RN52::setDiscoverable(bool discoverable) {
  enterCommandMode();
  if (discoverable) {
    serial->print("@,1\r");
  } else {
    serial->print("@,0\r");
  }
  waitForResponse();
  exitCommandMode();
}

void RN52::attemptReconnect() {
  serial->print("B\r");
  waitForResponse();
}

void RN52::disconnect() {
  serial->print("K,E");
  waitForResponse();
}

bool RN52::incomingCall() {
  return state.connectionStatus == 11;
}

void RN52::acceptCall() {
  serial->print("C\r");
  waitForResponse();
}

void RN52::rejectCall() {
  serial->print("E\r");
  waitForResponse();
}

void RN52::increaseVolume() {
  serial->print("AV+\r");
  waitForResponse();
}

void RN52::decreaseVolume() {
  serial->print("AV-\r");
  waitForResponse();
}

void RN52::nextTrack() {
  serial->print("AT+\r");
  waitForResponse();
}

void RN52::previousTrack() {
  serial->print("AT-\r");
  waitForResponse();
}

void RN52::playPause() {
  serial->print("AP\r");
  waitForResponse();
}

int RN52::getConnectionStatus() {
  return state.connectionStatus;
}

int RN52::ASCIIToNum(char c) {
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'F') {
    return (c - 'A' + 0xA);
  } else if (c >= 'a' && c <= 'f') {
    return (c - 'a' + 0xA);
  } else {
    return -1;
  }
}