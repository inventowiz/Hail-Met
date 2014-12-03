#include "RN52.h"
#include <Arduino.h>

void RN52::updateConnectionStatus() {
  int val;
  char inChar;

  enterCommandMode();
  serial->print("Q\r");
  String res = waitForResponse();
  exitCommandMode();

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

  pinMode(this->gpio9Pin, OUTPUT);
  digitalWrite(this->gpio9Pin, HIGH);
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

void RN52::sendCommand(String str, bool requireConnected) {
  if (!requireConnected || isConnected()) {  
    enterCommandMode();
    serial->print(str);
    waitForResponse();
    exitCommandMode();
  }
}

String RN52::waitForResponse() {
  return serial->readStringUntil(terminatingChar);
}

bool RN52::isDiscoverable() {
  return state.connectionStatus == 2;
}

void RN52::setDiscoverable(bool discoverable) {
  if (discoverable) {
    sendCommand("@,1\r", false);
  } else {
    sendCommand("@,0\r", false);
  }
}

void RN52::attemptReconnect() {
  sendCommand("B\r", false);
}

void RN52::disconnect() {
  sendCommand("K,E", true);
}

void RN52::reboot() {
  enterCommandMode();
  serial->print("R,1\r");
  exitCommandMode();
}

bool RN52::isConnected() {
  return state.connectionStatus > 2;
}

bool RN52::incomingCall() {
  return state.connectionStatus == 5;
}

bool RN52::activeCall() {
  return state.connectionStatus == 6;
}

void RN52::acceptCall() {
  sendCommand("C\r", true);
}

void RN52::rejectCall() {
  sendCommand("E\r", true);
}

void RN52::increaseVolume() {
  sendCommand("AV+\r", true);
}

void RN52::decreaseVolume() {
  sendCommand("AV-\r", true);
}

void RN52::nextTrack() {
  sendCommand("AT+\r", true);
}

void RN52::previousTrack() {
  sendCommand("AT-\r", true);
}

void RN52::playPause() {
  sendCommand("AP\r", true);
}

void RN52::activateVoiceCommand() {
  sendCommand("P\r", true);
}

int RN52::getConnectionStatus() {
  return state.connectionStatus;
}

void RN52::configSettings() {
  enterCommandMode();
  serial->print("S%,2003\r");
  waitForResponse();
  serial->print("SN,Hail-Met\r");
  waitForResponse();
  serial->print("SS,0F\r");
  waitForResponse();
  exitCommandMode();
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
