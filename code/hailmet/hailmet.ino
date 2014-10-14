#include <RN52.h>
#include <PinChangeInt.h>

int led = 13;
RN52 bt;

bool setDisc = false;
bool checkState = false;

void setDiscoverable() {
  setDisc = true;
}

void btStateChanged() {
  checkState = true;
}

void stateChanged() {
  int connStatus;
  if (!bt.isInCommandMode()) {
    bt.enterCommandMode();
  }

  bt.updateConnectionStatus();
  connStatus = bt.getConnectionStatus();
  
  switch(connStatus) {
    case 1:
    case 2:
      digitalWrite(13, LOW);
    break;

    case 3:
      digitalWrite(13, HIGH);
    break;
  }

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
  pinMode(led, OUTPUT);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  pinMode(2, INPUT); digitalWrite(2, HIGH);
  pinMode(8, INPUT); digitalWrite(8, HIGH);
  PCintPort::attachInterrupt(2, &setDiscoverable, FALLING);
  PCintPort::attachInterrupt(8, &btStateChanged, FALLING);
  Serial.begin(115200);
  delay(5);
  Serial.setTimeout(60000);
  bt.init(Serial, 4);
}

void loop() {
  if (setDisc) {
    bt.setDiscoverable(true);
    setDisc = false;
  }
  
  if (checkState) {
    stateChanged();
    checkState = false;
  }
}
