#include "PinChangeInt.h"
#include "TimerOne.h"
#include <SPI.h>
#include "Bounce2.h"
#include "RN52.h"
#include "LedDriver.h"
#include "HelmetIO.h"

#define PIN_SIGNAL_LEFT 2
#define PIN_SIGNAL_RIGHT 3
#define PIN_LED_DRIVER_LATCH 12
#define PIN_LED_DRIVER_ENABLE 6
#define PIN_BT_COMMAND_TOGGLE 10
#define PIN_BT_EVENT_CHANGE 8
#define PIN_BT_DISCOVERABLE 4
#define PIN_BT_CONNECTED 7
#define PIN_LIGHT_SENSOR A0

#define HEADLIGHT_MIN_BRIGHTNESS 25

RN52 bt;
LedDriver ledDriver;
HelmetIO helmet;

Bounce signalLeft = Bounce();
Bounce signalRight = Bounce();
Bounce setBtDiscoverable = Bounce();
int prevSetBtDiscoverable = 0;
int connLightState = 0;

bool setDiscoverable = false;
bool checkState = false;

int lightSensorVal;

void timerInterrupt() {
  helmet.updateLights();
  
  int connStatus = bt.getConnectionStatus();
  
  switch(connStatus) {
    case 1:
      digitalWrite(PIN_BT_CONNECTED, LOW);
      connLightState = 0;      
    break;
    case 2:
      digitalWrite(PIN_BT_CONNECTED, connLightState);
      connLightState = !connLightState;
    break;

    case 3:
      digitalWrite(PIN_BT_CONNECTED, HIGH);
      connLightState = 1;
    break;
  }
  
}

void btStateChanged() {
  checkState = true;
}

void handleBtStateChange() {
  int connStatus;
  if (!bt.isInCommandMode()) {
    bt.enterCommandMode();
  }

  bt.updateConnectionStatus();

  if (connStatus == 14) {
    // Turn on low battery indicator light
  } else {
    // Turn off low battery indicator light
  }

  if (bt.isInCommandMode()) {
    bt.exitCommandMode();
  }
}

void setup() {
  pinMode(PIN_SIGNAL_LEFT, INPUT);            // Initialize Left Turn Signal Button
  digitalWrite(PIN_SIGNAL_LEFT, HIGH);

  pinMode(PIN_SIGNAL_RIGHT, INPUT);            // Initialize Right Turn Signal Button
  digitalWrite(PIN_SIGNAL_RIGHT, HIGH);

  signalLeft.attach(PIN_SIGNAL_LEFT);        // Attach turn signal interrupts
  signalRight.attach(PIN_SIGNAL_RIGHT);
  setBtDiscoverable.attach(PIN_BT_DISCOVERABLE);

  signalLeft.interval(20);
  signalRight.interval(20);
  setBtDiscoverable.interval(20);
  
  pinMode(PIN_BT_CONNECTED, OUTPUT);          // Initialize Bluetooth Connected LED
  digitalWrite(PIN_BT_CONNECTED, LOW);
  
  pinMode(PIN_BT_EVENT_CHANGE, INPUT);          // Initialize Bluetooth Event Change Button
  digitalWrite(PIN_BT_EVENT_CHANGE, HIGH);
  PCintPort::attachInterrupt(PIN_BT_EVENT_CHANGE, &btStateChanged, FALLING);
  
  pinMode(PIN_BT_DISCOVERABLE, INPUT);
  digitalWrite(PIN_BT_DISCOVERABLE, HIGH);
  
  pinMode(PIN_LIGHT_SENSOR, INPUT);

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
  
  
  signalLeft.update();
  signalRight.update();
  setBtDiscoverable.update();

  if (signalLeft.read()) {
    // Active low!
    helmet.disableLeftTurnSignal();
  } else {
    helmet.enableLeftTurnSignal();
  }
  
  if (signalRight.read()) {
    // Active low!
    helmet.disableRightTurnSignal();
  } else {
    helmet.enableRightTurnSignal();
  }

  
  if (!setBtDiscoverable.read() && prevSetBtDiscoverable) {
    bt.setDiscoverable(true);
  }
  
  prevSetBtDiscoverable = setBtDiscoverable.read();
}
