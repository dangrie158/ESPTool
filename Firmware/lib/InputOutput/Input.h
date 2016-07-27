#include <Arduino.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef INPUT_H_
#define INPUT_H_

class Input {
protected:
  uint8_t mPin;
  bool mStatus;
  bool mActiveLow;

public:
  Input(uint8_t pinNumber, bool internalPullupEnabled = false,
        bool activeLow = false);

  virtual uint16_t read();
};

#endif // INPUT_H_
