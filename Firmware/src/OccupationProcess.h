#ifndef OCCUPATION_PROCESS_H_
#define OCCUPATION_PROCESS_H_

#include "ScanProcess.h"

// Name of the Process used in all Screens
#define OCCUPATION_PROCESS_NAME "Channel Occupation"

#define WLAN_NUM_CHANNELS 13

class OccupationProcess : public ScanProcess {

public:
  OccupationProcess(SSD1306 *display);

  void handleInput(button_t button, status_t action);
  void showResult(int numNetworksDiscovered);
};

#endif // OCCUPATION_PROCESS_H_
