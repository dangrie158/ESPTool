#include "Input.h"
#include <Arduino.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef BUTTON_H_
#define BUTTON_H_

typedef enum {
  Open = 0,
  Closed,
  Held,
  Clicked,
  Released,
  DoubleClicked
} status_t;

class Button : Input {
private:
  volatile status_t mCurrentStatus;

  uint16_t mKeyDownTicks;
  uint16_t mDoubleClickTicks;
  unsigned long mLastButtonCheck;
  bool mDoubleClickEnabled;

public:
  Button(uint8_t pinNumber, bool internalPullupEnabled = false,
         bool activeLow = false, bool doubleClickEnabled = false);

  status_t getStatus();

  void update();
};

#endif // BUTTON_H_
