#include "Button.h"

// ----------------------------------------------------------------------------
// Button configuration (values for 1ms timer service calls)
//
#define ENC_BUTTONINTERVAL 10   // debounce time
#define ENC_DOUBLECLICKTIME 600 // second click within 600ms
#define ENC_HOLDTIME 1200       // report held button after 1.2s

Button::Button(uint8_t pinNumber, bool internalPullupEnabled, bool activeLow)
    : Input(pinNumber, activeLow, internalPullupEnabled), mCurrentStatus(Open),
      mKeyDownTicks(0), mLastButtonCheck(0) {}

status_t Button::getStatus() {
  status_t status = mCurrentStatus;
  // only fire click event once
  if (status == Clicked || status == Released) {
    mCurrentStatus = Open;
  }
  return status;
}

void Button::update() {
  unsigned long now = millis();

  mLastButtonCheck = now;

  if (this->read()) { // key is down
    mKeyDownTicks++;
    if (mKeyDownTicks > (ENC_HOLDTIME / ENC_BUTTONINTERVAL)) {
      this->mCurrentStatus = Held;
    } else {
      this->mCurrentStatus = Closed;
    }
  } else { // key is now up
    if (mKeyDownTicks) {
      if (this->mCurrentStatus == Held) {
        this->mCurrentStatus = Released;
      } else {
        this->mCurrentStatus = Clicked;
      }
      mKeyDownTicks = 0;
    }
  }
}
