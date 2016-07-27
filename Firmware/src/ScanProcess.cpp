#include "ScanProcess.h"
#include "ESP8266WiFi.h"
#include "ListScreen.h"
#include "Process.h"

ScanProcess::ScanProcess(SSD1306 *display)
    : Process(new ListScreen(PROCESS_NAME, display)) {}

void ScanProcess::initialize() {
  WiFi.mode(WIFI_STA);
  // WiFi.disconnect();
  delay(100);

  // TODO: replace with async and poll scanComplete()
  int n = WiFi.scanNetworks(false, true);
  fillList(n);
}

Process *ScanProcess::update() { mScreen->draw(); }

void ScanProcess::fillList(int numberOfNetworks) {
  ListScreen *screen = static_cast<ListScreen *>(this->mScreen);
  for (int i = 0; i < numberOfNetworks; i++) {
    String ssid = WiFi.SSID(i);
    String rssi = String(WiFi.RSSI(i)) + "dBm";
    ListItem *item = new ListItem(ssid, String(i), true, rssi);
    screen->addItem(item);
  }
  ListItem *rescan =
      new ListItem(PSTR("     ---- Scan again ----"), "-1", true);
  screen->addItem(rescan);
}

void ScanProcess::handleInput(button_t button, status_t action) {
  ListScreen *screen = static_cast<ListScreen *>(this->mScreen);
  if (action == Clicked) {
    switch (button) {
    case Up:
      screen->cursorUp();
      break;
    case Down:
      screen->cursorDown();
      break;
    case Left:
      // ToDo: connect or whatever...
      break;
    }
  }
}
