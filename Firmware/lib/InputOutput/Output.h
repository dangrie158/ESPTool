#include <Arduino.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef OUPUT_H_
#define OUPUT_H_

class Output {
private:
  uint8_t mPin;
  bool mStatus;
  bool mActiveLow;

public:
  Output(uint8_t pinNumber, bool activeLow = false);

  void switchOn();
  void switchOff();
  void toggle();
  bool getStatus();
};

#endif // OUPUT_H_
