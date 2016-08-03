#include "OccupationProcess.h"
#include "BarGraphScreen.h"
#include "ESP8266WiFi.h"

OccupationProcess::OccupationProcess(SSD1306 *display) : ScanProcess(display) {}

void OccupationProcess::showResult(int numNetworksDiscovered) {
  // switch to a new ListScreen to show the result
  Screen *oldScreen = this->mScreen;
  BarGraphScreen *screen = new BarGraphScreen(
      this->mDisplay, OCCUPATION_PROCESS_NAME, WLAN_NUM_CHANNELS);
  this->mScreen = screen;
  delete oldScreen;

  // fill the list
  for (int i = 0; i < numNetworksDiscovered; i++) {
    int channel = WiFi.channel(i);
    int lastBarValue = screen->getBarValue(channel - 1);
    screen->setBarValue(channel - 1, lastBarValue + 1);
  }

  // fill in the labels
  for (int i = 0; i < WLAN_NUM_CHANNELS; i++) {
    screen->setBarLabel(i, String(i + 1));
  }
}

void OccupationProcess::handleInput(button_t button, status_t action) {
  // make sure we're displaying a ListScreen before casting
  if (mScanComplete) {
    BarGraphScreen *screen = static_cast<BarGraphScreen *>(this->mScreen);
    if (action == Clicked) {
      switch (button) {
      case Up:

        break;
      case Down:

        break;
      case Left:

        break;
      }
    }
  }
}
