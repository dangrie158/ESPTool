#ifndef PROCESS_H_
#define PROCESS_H_

#include "Button.h"
#include "Screen.h"
#include <Arduino.h>

typedef enum { Up = 0, Down, Left } button_t;

class Process {
protected:
  Screen *mScreen;
  const String mName;
  void (*mCallback)(int);

public:
  Process(const String name, Screen *screen) : mName(name), mScreen(screen){};
  virtual ~Process() { delete mScreen; }
  virtual void initialize() {}
  virtual Process *update() {}
  virtual void handleInput(button_t button, status_t Action) {}
  inline void setCallback(void (*callback)(int)) { mCallback = callback; }
  inline void setScreen(Screen *newScreen) { mScreen = newScreen; }
  inline const String getName() { return mName; }
};

#endif // PROCESS_H_
