#ifndef _BARGRAPHSCREEN_H_
#define _BARGRAPHSCREEN_H_

#include "SSD1306.h"
#include "Screen.h"
#include <Arduino.h>

class BarGraphScreen : public Screen {
private:
  int mNumBars;
  int *mBarValues;
  String *mBarTitles;
  String mMessage;

public:
  BarGraphScreen(SSD1306 *display, const String message, int numBars);
  ~BarGraphScreen();
  void draw();
  inline void setBarValue(int bar, int value) { mBarValues[bar] = value; }
  inline void setBarLabel(int bar, String title) { mBarTitles[bar] = title; }
  inline int getBarValue(int bar) { return mBarValues[bar]; }
};

#endif //_BARGRAPHSCREEN_H_
