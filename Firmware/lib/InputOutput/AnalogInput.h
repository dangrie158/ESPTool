#include "Input.h"
#include <Arduino.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef ANALOG_INPUT_H_
#define ANALOG_INPUT_H_

// 10 bit ADC
#define ADC_MAX 1024

// default scaling factor is to map the 0-1024 values in a range of 0-1
#define DEFAULT_CONVERSION 1.0f / ADC_MAX

class AnalogInput : Input {
private:
  float mConversionFactor;
  uint8_t mNumSamples;

public:
  AnalogInput(uint8_t pinNumber, float conversionFactor = DEFAULT_CONVERSION,
              uint8_t numSamples = 1);

  uint16_t read();
  float readConverted();
};

#endif // ANALOG_INPUT_H_
