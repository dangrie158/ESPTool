#ifndef SCAN_PROCESS_H_
#define SCAN_PROCESS_H_

#include "Process.h"

#define PROCESS_NAME "Scan for Networks"

class ScanProcess : public Process {
private:
public:
  ScanProcess(SSD1306 *display);
  void initialize();
  Process *update();
  void handleInput(button_t button, status_t action);
  void fillList(int);
};

#endif // SCAN_PROCESS_H_
