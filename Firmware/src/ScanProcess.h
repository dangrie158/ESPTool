#ifndef SCAN_PROCESS_H_
#define SCAN_PROCESS_H_

#include "Process.h"

// Name of the Process used in all Screens
#define PROCESS_NAME "Scan for Networks"

// message used on the loading screen
#define SCAN_MESSAGE "scanning..."

// Identifier for the ListItem to rescan for networks
#define RESCAN "-1"

class ScanProcess : public Process {
private:
  bool mScanComplete;

public:
  ScanProcess(SSD1306 *display);
  ~ScanProcess();
  void initialize();
  Process *update();
  void handleInput(button_t button, status_t action);
  void fillList(int);
};

#endif // SCAN_PROCESS_H_
