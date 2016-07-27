#include "Input.h"

Input::Input(uint8_t pinNumber, bool internalPullupEnabled, bool activeLow)
    : mPin(pinNumber), mStatus(false), mActiveLow(activeLow) {
  pinMode(this->mPin, internalPullupEnabled ? INPUT_PULLUP : INPUT);
  digitalWrite(this->mPin, mActiveLow);
}

uint16_t Input::read() {
  bool state = digitalRead(mPin) ? !mActiveLow : mActiveLow;
  return state;
}
