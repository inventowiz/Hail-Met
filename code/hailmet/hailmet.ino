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
#define PIN_BT_ACCEPT_REJECT A3
#define PIN_BT_ENABLE 9
#define PIN_HEADLIGHT 5

#define HEADLIGHT_MIN_BRIGHTNESS 10
#define HEADLIGHT_MAX_BRIGHTNESS 128
#define HEADLIGHT_MIN_TRIGGER 20
#define HEADLIGHT_MAX_TRIGGER 35
#define BTN_HELD_LENGTH 7500
#define BTN_LONG_HELD_LENGTH 15000

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

bool connLightState = false;
int connLightDelay = 0;

bool checkState = false;
int stateChangeCount = 0;

bool programMode = false;

int lightSensorVal;

void timerInterrupt() {
  helmet.updateLights();  
  
  if (programMode) {
    if (connLightDelay == 0) {
      digitalWrite(PIN_BT_CONNECTED, HIGH);
      connLightState = true;
    } else {
      digitalWrite(PIN_BT_CONNECTED, LOW);
      connLightState = false;
    }
    
    if (connLightDelay >= 3) {
      connLightDelay = 0;
    } else {
      connLightDelay++;
    }
  } else if (bt.getConnectionStatus() == 2) {
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
  stateChangeCount++;
}

void setProgramMode() {
  connLightDelay = 0;
  programMode = true;
  digitalWrite(PIN_BT_ENABLE, LOW);
  bt.reboot();
}

void setBtDiscoverable(bool held) {
  if (!held) {
    bt.setDiscoverable(true);
  }
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
  
  if (bt.incomingCall()) {
    helmet.setCallStatus(INCOMING_CALL);
  } else if (bt.activeCall()) {
    helmet.setCallStatus(ACTIVE_CALL);
  } else {
    helmet.setCallStatus(NO_CALL);
  }
}

void setup() {
  // Initialize Left Turn Signal Button
  pinMode(PIN_SIGNAL_LEFT, INPUT);
  digitalWrite(PIN_SIGNAL_LEFT, HIGH);

  // Initialize Right Turn Signal Button
  pinMode(PIN_SIGNAL_RIGHT, INPUT);
  digitalWrite(PIN_SIGNAL_RIGHT, HIGH);
  
  // Initialize Bluetooth Connected LED
  pinMode(PIN_BT_CONNECTED, OUTPUT);
  digitalWrite(PIN_BT_CONNECTED, LOW);
  
  // Initialize Bluetooth Enable
  pinMode(PIN_BT_ENABLE, OUTPUT);
  digitalWrite(PIN_BT_ENABLE, HIGH);
  
  // Initialize Bluetooth Event Change
  pinMode(PIN_BT_EVENT_CHANGE, INPUT);
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
  btnBtDiscoverable.init(&btDiscoverable, true, BTN_LONG_HELD_LENGTH, NULL, setBtDiscoverable, setProgramMode);
  btnBtAction.init(&btAction, true, BTN_HELD_LENGTH, NULL, btnBtActionReleased, btnBtActionHeld);

  Serial.begin(115200);
  Serial.setTimeout(60000);
  bt.init(Serial, PIN_BT_COMMAND_TOGGLE);
  handleBtStateChange();

  ledDriver.init(PIN_LED_DRIVER_LATCH, PIN_LED_DRIVER_ENABLE);
  
  helmet.init(ledDriver, PIN_HEADLIGHT);
  helmet.enableTaillight();
  helmet.updateLights();

  Timer1.initialize(500000);
  Timer1.attachInterrupt(timerInterrupt);
}

void loop() {
  if (stateChangeCount > 0) {
    handleBtStateChange();
    stateChangeCount--;
  }
  
  lightSensorVal = analogRead(PIN_LIGHT_SENSOR);
  lightSensorVal = map(lightSensorVal, 0, 1023, 0, 255);
  ledDriver.setBrightness(max(255 - lightSensorVal, 30));
  int adjustedLightVal = map(lightSensorVal, 0, HEADLIGHT_MAX_TRIGGER, 0, HEADLIGHT_MAX_BRIGHTNESS);
  if (helmet.getHeadlight()) {
    if (lightSensorVal > HEADLIGHT_MAX_TRIGGER) {
      helmet.disableHeadlight();
    } else {
      helmet.setHeadlightBrightness(max(HEADLIGHT_MIN_BRIGHTNESS, HEADLIGHT_MAX_BRIGHTNESS - adjustedLightVal));
    }
  } else {
    if (lightSensorVal < HEADLIGHT_MIN_TRIGGER) {
      helmet.setHeadlightBrightness(max(HEADLIGHT_MIN_BRIGHTNESS, HEADLIGHT_MAX_BRIGHTNESS - adjustedLightVal));
      helmet.enableHeadlight();
    }
  }
  
  btnSignalLeft.update();
  btnSignalRight.update();
  btnBtDiscoverable.update();
  btnBtAction.update(); 
}
