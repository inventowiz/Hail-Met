#include "PinChangeInt.h"
#include "TimerOne.h"
#include <SPI.h>
#include "Bounce2.h"
#include "RN52.h"
#include "LedDriver.h"
#include "HelmetIO.h"
#include "Button.h"

#define PIN_SIGNAL_LEFT 2
#define PIN_SIGNAL_RIGHT 3
#define PIN_LED_DRIVER_LATCH 12
#define PIN_LED_DRIVER_ENABLE 6
#define PIN_BT_COMMAND_TOGGLE 10
#define PIN_BT_EVENT_CHANGE 8
#define PIN_BT_DISCOVERABLE 4
#define PIN_BT_CONNECTED 7
#define PIN_LIGHT_SENSOR A0
#define PIN_BT_ACCEPT_REJECT 9

#define HEADLIGHT_MIN_BRIGHTNESS 25
#define BTN_HELD_LENGTH 7500

RN52 bt;
LedDriver ledDriver;
HelmetIO helmet;

Bounce signalLeft = Bounce();
Bounce signalRight = Bounce();
Bounce btDiscoverable = Bounce();
Bounce btAction = Bounce();

Button btnSignalLeft;
Button btnSignalRight;
Button btnBtDiscoverable;
Button btnBtAction;

int connLightState = 0;

bool checkState = false;

int lightSensorVal;

void timerInterrupt() {
  helmet.updateLights();  
  
  if (bt.getConnectionStatus() == 2) {
    digitalWrite(PIN_BT_CONNECTED, connLightState);
    connLightState = !connLightState;
  }
}

void toggleLeftTurnSignal() {
  if (helmet.getLeftTurnSignal()) {
    helmet.disableLeftTurnSignal();
  } else {
    helmet.enableLeftTurnSignal();
  }
}

void toggleRightTurnSignal() {
  if (helmet.getRightTurnSignal()) {
    helmet.disableRightTurnSignal();
  } else {
    helmet.enableRightTurnSignal();
  }
}

void btStateChanged() {
  checkState = true;
}

void setBtDiscoverable() {
  bt.setDiscoverable(true);
}

void btnBtActionHeld() {
  if (bt.incomingCall()) {
    bt.rejectCall();
  }
}

void btnBtActionReleased(bool held) {
  if (bt.incomingCall()) {
    if (!held) {
      bt.acceptCall();
    } 
  } else if (bt.activeCall()) {
    bt.rejectCall();
  } else {  
    bt.activateVoiceCommand();
  }
}

void handleBtStateChange() {
  bt.updateConnectionStatus();

  int connStatus = bt.getConnectionStatus();
  
  switch(connStatus) {
    case 1:
      digitalWrite(PIN_BT_CONNECTED, LOW);
      connLightState = 0;      
    break;

    case 3:
      digitalWrite(PIN_BT_CONNECTED, HIGH);
      connLightState = 1;
    break;
  }
}

void setup() {
  pinMode(PIN_SIGNAL_LEFT, INPUT);            // Initialize Left Turn Signal Button
  digitalWrite(PIN_SIGNAL_LEFT, HIGH);

  pinMode(PIN_SIGNAL_RIGHT, INPUT);            // Initialize Right Turn Signal Button
  digitalWrite(PIN_SIGNAL_RIGHT, HIGH);
  
  pinMode(PIN_BT_CONNECTED, OUTPUT);          // Initialize Bluetooth Connected LED
  digitalWrite(PIN_BT_CONNECTED, LOW);
  
  pinMode(PIN_BT_EVENT_CHANGE, INPUT);          // Initialize Bluetooth Event Change Button
  digitalWrite(PIN_BT_EVENT_CHANGE, HIGH);
  PCintPort::attachInterrupt(PIN_BT_EVENT_CHANGE, &btStateChanged, FALLING);
  
  pinMode(PIN_BT_ACCEPT_REJECT, INPUT);
  digitalWrite(PIN_BT_ACCEPT_REJECT, HIGH);
  
  pinMode(PIN_BT_DISCOVERABLE, INPUT);
  digitalWrite(PIN_BT_DISCOVERABLE, HIGH);
  
  pinMode(PIN_LIGHT_SENSOR, INPUT);
  
  // Set up the button debouncers
  signalLeft.attach(PIN_SIGNAL_LEFT);
  signalRight.attach(PIN_SIGNAL_RIGHT);
  btDiscoverable.attach(PIN_BT_DISCOVERABLE);
  btAction.attach(PIN_BT_ACCEPT_REJECT);
  
  // Set debouncing interval for each debouncer
  signalLeft.interval(20);
  signalRight.interval(20);
  btDiscoverable.interval(20);
  btAction.interval(20);
  
  // Set up buttons
  btnSignalLeft.init(&signalLeft, true, BTN_HELD_LENGTH, toggleLeftTurnSignal, NULL, NULL);
  btnSignalRight.init(&signalRight, true, BTN_HELD_LENGTH, toggleRightTurnSignal, NULL, NULL);
  btnBtDiscoverable.init(&btDiscoverable, true, BTN_HELD_LENGTH, setBtDiscoverable, NULL, NULL);
  btnBtAction.init(&btAction, true, BTN_HELD_LENGTH, NULL, btnBtActionReleased, btnBtActionHeld);

  Serial.begin(115200);
  Serial.setTimeout(60000);
  bt.init(Serial, PIN_BT_COMMAND_TOGGLE);
  handleBtStateChange();

  ledDriver.init(PIN_LED_DRIVER_LATCH, PIN_LED_DRIVER_ENABLE);
  
  helmet.init(ledDriver);
  helmet.enableTaillight();
  helmet.updateLights();

  Timer1.initialize(500000);
  Timer1.attachInterrupt(timerInterrupt);
}

void loop() {
  if (checkState) {
    handleBtStateChange();
    checkState = false;
  }
  
  lightSensorVal = analogRead(PIN_LIGHT_SENSOR);
  lightSensorVal = map(lightSensorVal, 0, 1023, 0, 255);
  ledDriver.setBrightness(max(255 - lightSensorVal, 30));
  
  if (lightSensorVal < 50) {
    helmet.setHeadlightBrightness(max(HEADLIGHT_MIN_BRIGHTNESS, lightSensorVal));
    helmet.enableHeadlight();
  } else {
    helmet.disableHeadlight();
  }
  
  btnSignalLeft.update();
  btnSignalRight.update();
  btnBtDiscoverable.update();
  btnBtAction.update(); 
}
