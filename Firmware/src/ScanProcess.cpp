#include "ScanProcess.h"
#include "ESP8266WiFi.h"
#include "ListScreen.h"
#include "LoadingScreen.h"
#include "Process.h"

ScanProcess::ScanProcess(SSD1306 *display)
    : Process(SCAN_PROCESS_NAME, NULL), mDisplay(display) {}

ScanProcess::~ScanProcess() { delete mScreen; }

void ScanProcess::initialize() { startScan(); }

void ScanProcess::startScan() {
  Screen *tempScreen = this->mScreen;
  LoadingScreen *screen = new LoadingScreen(mDisplay, SCAN_MESSAGE);
  this->mScreen = screen;
  if (tempScreen != NULL) {
    delete tempScreen;
  }

  mScanComplete = false;
  WiFi.mode(WIFI_STA);
  delay(100);

  WiFi.scanNetworks(true, true);
  screen->setProgress(INDETERMINATE);
}

void ScanProcess::showResult(int numNetworksDiscovered) {
  // switch to a new ListScreen to show the result
  Screen *oldScreen = this->mScreen;
  this->mScreen = new ListScreen(oldScreen->getDisplay());
  delete oldScreen;

  // fill the list
  fillList(numNetworksDiscovered);
}

Process *ScanProcess::update() {
  int scanStatus = WiFi.scanComplete();
  if (scanStatus >= 0 && !mScanComplete) {
    showResult(scanStatus);
    mScanComplete = true;
  } else if (scanStatus == WIFI_SCAN_FAILED) {

  } else if (scanStatus == WIFI_SCAN_RUNNING) {
  }

  mScreen->draw();
}

void ScanProcess::fillList(int numberOfNetworks) {
  ListScreen *screen = static_cast<ListScreen *>(this->mScreen);
  for (int i = 0; i < numberOfNetworks; i++) {
    String ssid = WiFi.SSID(i);
    String encryptionScheme =
        WiFi.encryptionType(i) == ENC_TYPE_NONE ? "  " : " *";
    String rssi = String(WiFi.RSSI(i));
    ListItem *item =
        new ListItem(ssid, String(i), true, rssi + encryptionScheme);
    screen->addItem(item);
  }
  ListItem *rescan =
      new ListItem(PSTR("     ---- Scan again ----"), RESCAN, true);
  screen->addItem(rescan);
}

void ScanProcess::handleInput(button_t button, status_t action) {
  // make sure we're displaying a ListScreen before casting
  if (mScanComplete) {
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
        ListItem *selectedItem = screen->getSelectedItem();
        if (selectedItem->getReference() == RESCAN) {
          this->initialize();
        } else {
          if (mCallback != NULL) {
            mCallback(selectedItem->getReference().toInt());
          }
        }
        break;
      }
    }
  }
}
