#include "Input.h"
#include <Arduino.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef BUTTON_H_
#define BUTTON_H_

typedef enum { Open = 0, Closed, Held, Clicked } status_t;

class Button : Input {
private:
  volatile status_t mCurrentStatus;

  uint16_t mKeyDownTicks;
  unsigned long mLastButtonCheck;

public:
  Button(uint8_t pinNumber, bool internalPullupEnabled = false,
         bool activeLow = false);

  status_t getStatus();

  void update();
};

#endif // BUTTON_H_
