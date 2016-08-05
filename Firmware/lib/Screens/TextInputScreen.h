#ifndef _TEXTINPUTSCREEN_H_
#define _TEXTINPUTSCREEN_H_

#include "SSD1306.h"
#include "Screen.h"
#include <Arduino.h>

// the first char in ascii that is not a control char (space)
#define FIRST_CHAR 0x20

// the last char in ascii that is a valid character
#define LAST_CHAR 0x7E
#define START_CHAR 0x41

class TextInputScreen : public Screen {
private:
  const String mMessage;
  String mInput;
  int mCurrentCharPos;

public:
  TextInputScreen(SSD1306 *display, const String message);
  ~TextInputScreen();
  void draw();

  inline String getInputString() { return mInput; }
  inline String setInputString(const String &text) {
    mInput = text;
    mCurrentCharPos = text.length() - 1;
  }

  inline void nextChar() {
    // duplicate the last char, that makes the input easier
    // because if you enter a long numeric password
    // or a sequence of special chars you're already
    // in the right region of the ascii table
    char lastChar = mInput.charAt(mCurrentCharPos);
    mInput += lastChar;
    mCurrentCharPos++;
  }

  inline void prevChar() {
    if (mInput.length() > 1) {
      mInput.remove(mInput.length() - 1);
      mCurrentCharPos--;
    }
  }

  inline void charUp() {
    char current = mInput.charAt(mCurrentCharPos);
    char newChar = current + 1 > LAST_CHAR ? FIRST_CHAR : current + 1;
    mInput.setCharAt(mCurrentCharPos, newChar);
  }

  inline void charDown() {
    char current = mInput.charAt(mCurrentCharPos);
    char newChar = current - 1 < FIRST_CHAR ? LAST_CHAR : current - 1;
    mInput.setCharAt(mCurrentCharPos, newChar);
  }
};

#endif //_TEXTINPUTSCREEN_H_
