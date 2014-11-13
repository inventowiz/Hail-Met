#include "Button.h"

void Button::init(Bounce *bouncer, bool activeLow, int heldThreshold, void (*pressed)(void), void (*released)(bool), void (*held)(void)) {
  this->bouncer = bouncer;
  this->activeLow = activeLow;
  this->heldThreshold = heldThreshold;
  this->pressed = pressed;
  this->released = released;
  this->held = held;
  btnPressed = false;
  btnIgnore = false;
  btnHeld = false;
  btnHeldCount = 0;
}
  
void Button::update() {
  bool state;
  
  bouncer->update();

  if (activeLow) {
    state = !bouncer->read();
  } else {
    state = bouncer->read();;
  }
  
  if (state) {
    // btAction is pressed  
    if (!btnPressed) {
      // btAction was not pressed before
      btnHeldCount = 0;
      btnHeld = false;
      btnPressed = true;
      if (pressed) {
        pressed();
      }
    } else {
      // btAction was pressed before
      // If the btAction has been pressed long enough to be considered "held"
      if (btnHeldCount > heldThreshold) {
        if (!btnIgnore) {
          if (held) {
            held();
            btnHeld = true;
          }
          btnIgnore = true;
        }
      } else {
        btnHeldCount++; 
      }
    }
  } else {
    // btAction is not pressed
    btnIgnore = false;
    
    if (btnPressed) {
      // btAction was pressed before
      if (released) {
        released(btnHeld);
      }
      btnPressed = false;
    }
  }
}
  
