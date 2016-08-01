#include "Button.h"

// ----------------------------------------------------------------------------
// Button configuration (values for 1ms timer service calls)
//
#define ENC_BUTTONINTERVAL 10   // debounce time
#define ENC_DOUBLECLICKTIME 400 // second click within 600ms
#define ENC_HOLDTIME 1200       // report held button after 1.2s

Button::Button(uint8_t pinNumber, bool internalPullupEnabled, bool activeLow,
               bool doubleClickEnabled)
    : Input(pinNumber, activeLow, internalPullupEnabled), mCurrentStatus(Open),
      mKeyDownTicks(0), mLastButtonCheck(0),
      mDoubleClickEnabled(doubleClickEnabled) {}

status_t Button::getStatus() {
  status_t status = mCurrentStatus;
  // only fire click event once
  if (status == Clicked || status == Released || status == DoubleClicked) {
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
        mDoubleClickTicks = 0;
      } else {
        if (mDoubleClickTicks > 1) {
          if (mDoubleClickTicks < (ENC_DOUBLECLICKTIME / ENC_BUTTONINTERVAL)) {
            this->mCurrentStatus = DoubleClicked;
            mDoubleClickTicks = 0;
          }
        } else {
          mDoubleClickTicks = (mDoubleClickEnabled)
                                  ? (ENC_DOUBLECLICKTIME / ENC_BUTTONINTERVAL)
                                  : 1;
        }
      }
    }
    mKeyDownTicks = 0;
  }
  if (mDoubleClickTicks > 0) {
    mDoubleClickTicks--;
    if (mDoubleClickTicks == 0) {
      this->mCurrentStatus = Clicked;
    }
  }
}
