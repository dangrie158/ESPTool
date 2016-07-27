#include "AnalogInput.h"

AnalogInput::AnalogInput(uint8_t pinNumber, float conversionFactor,
                         uint8_t numSamples)
    : Input(pinNumber, false, false), mConversionFactor(conversionFactor),
      mNumSamples(numSamples) {
  pinMode(this->mPin, INPUT);
}

uint16_t AnalogInput::read() {
  uint32_t sum = 0;

  for (uint8_t i = 0; i < this->mNumSamples; ++i) {
    sum += analogRead(this->mPin);
  }

  return (uint16_t)(sum / this->mNumSamples);
}

float AnalogInput::readConverted() {
  uint16_t value = this->read();
  float convertedValue = value * this->mConversionFactor;
  return convertedValue;
}
