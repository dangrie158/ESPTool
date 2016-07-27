#ifndef PROCESS_H_
#define PROCESS_H_

#include "Button.h"
#include "Screen.h"
#include <Arduino.h>

typedef enum { Up = 0, Down, Left } button_t;

class Process {
protected:
  Screen *mScreen;

public:
  Process(Screen *screen) : mScreen(screen){};
  virtual ~Process() { delete mScreen; }
  virtual void initialize() {}
  virtual Process *update() {}
  virtual void handleInput(button_t button, status_t Action) {}
};

#endif // PROCESS_H_
