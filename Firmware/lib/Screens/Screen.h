#ifndef _SCREEN_H_
#define _SCREEN_H_

#include "SSD1306.h"
#include <Arduino.h>

#define SCREEN_HEIGHT 50
#define SCREEN_WIDTH 128
#define SCREEN_START_X 0
#define SCREEN_START_Y ((DISPLAY_HEIGHT) - (SCREEN_HEIGHT))

class Screen {
private:
  String mName;

protected:
  SSD1306 *mDisplay;

public:
  Screen(const String name, SSD1306 *display)
      : mName(name), mDisplay(display) {}
  virtual void draw() = 0;
  void clear() {
    mDisplay->setColor(BLACK);
    mDisplay->fillRect(SCREEN_START_X, SCREEN_START_Y, SCREEN_WIDTH,
                       SCREEN_HEIGHT);
    mDisplay->setColor(WHITE);
  }
  inline const String getName() { return mName; }

  inline String trimStringToLength(String input, int maxLength) {
    String empty = "";
    if (maxLength <= 0) {
      return empty;
    }

    while (mDisplay->getStringWidth(input) > maxLength) {
      int charsLeft = input.length();
      if (charsLeft <= 4) {
        return empty;
      }
      input.remove(charsLeft - 4);
      input += "...";
    }

    return input;
  }
};

#endif //_SCREEN_H_
