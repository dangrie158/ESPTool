#include "Output.h"

Output::Output(uint8_t pinNumber, bool activeLow)
    : mPin(pinNumber), mStatus(false), mActiveLow(activeLow) {
  pinMode(this->mPin, OUTPUT);
  digitalWrite(this->mPin, mActiveLow);
}

void Output::switchOn() {
  digitalWrite(this->mPin, !mActiveLow);
  this->mStatus = true;
}

void Output::switchOff() {
  digitalWrite(this->mPin, mActiveLow);
  this->mStatus = false;
}

void Output::toggle() {
  digitalWrite(this->mPin, this->mStatus ? mActiveLow : !mActiveLow);
  this->mStatus = !this->mStatus;
}

bool Output::getStatus() { return this->mStatus; }
