#ifndef BUTTON_H
#define BUTTON_H

#include "Bounce2.h"

class Button {
  Bounce *bouncer;
  bool activeLow;
  int heldThreshold;
  bool btnPressed;
  bool btnIgnore;
  bool btnHeld;
  int btnHeldCount;
  
  void (*pressed)(void);
  void (*released)(bool);
  void (*held)(void);
  
  public:
    void init(Bounce *bouncer, bool activeLow, int heldThreshold, void (*pressed)(void), void (*released)(bool), void (*held)(void));
    void update();
};

#endif
