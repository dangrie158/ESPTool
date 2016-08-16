#include "ScanHostsProcess.h"
#include "ESP8266WiFi.h"
#include "ListScreen.h"
#include "MessageScreen.h"
#include "Process.h"

ScanHostsProcess::ScanHostsProcess(SSD1306 *display)
    : Process(SCAN_HOST_PROCESS_NAME, NULL), mDisplay(display), mPinger(NULL) {}

ScanHostsProcess::~ScanHostsProcess() {
  if (mPinger != NULL) {
    delete mPinger;
  }
  delete mScreen;
}

void ScanHostsProcess::initialize() {
  IPAddress netMask = WiFi.subnetMask();
  IPAddress localIp = WiFi.localIP();

  mHost = IPAddress2ip_addr_t(localIp);
  mNetMask = IPAddress2ip_addr_t(netMask);
  mNetAddress.addr = mHost.addr & mNetMask.addr;

  showStartScreen();
}

void ScanHostsProcess::showStartScreen() {
  mStartAddress.addr = mNetAddress.addr + (1 << 24);
  mEndAddress.addr = (mNetAddress.addr | ~mNetMask.addr) - (1 << 24);

  char IpStrBuffer[16];
  String message = "Network: ";
  sprintf(IpStrBuffer, IPSTR, IP2STR(&mNetAddress));
  message += IpStrBuffer;
  message += "\n";
  sprintf(IpStrBuffer, IPSTR, IP2STR(&mStartAddress));
  message += IpStrBuffer;
  message += " -\n";
  sprintf(IpStrBuffer, IPSTR, IP2STR(&mEndAddress));
  message += IpStrBuffer;

  Screen *tempScreen = this->mScreen;
  MessageScreen *screen = new MessageScreen(mDisplay, message, "start scan");
  this->mScreen = screen;
  if (tempScreen != NULL) {
    delete tempScreen;
  }

  mScanStatus = NOT_STARTED;
}

void ScanHostsProcess::startScan() {
  Screen *tempScreen = this->mScreen;
  ListScreen *screen = new ListScreen(mDisplay, true);

  mCurrentHost.addr = mStartAddress.addr;

  // add the cancel item
  ListItem *cancelItem = new ListItem("cancel scan", CANCEL, true, "0%");
  screen->addItem(cancelItem);

  // change the screen
  this->mScreen = screen;

  // delete the old screen
  if (tempScreen != NULL) {
    delete tempScreen;
  }

  if (mPinger == NULL) {
    mPinger = new Pinger(10);
    mPinger->initialize();
  }

  mPinger->setCallback(&ScanHostsProcess::pingerCallback, this);
  mScanStatus = RUNNING;
}

void ScanHostsProcess::stopScan() {
  if (mPinger != NULL) {
    // delete the old pinger, we create a new one for each scan
    delete mPinger;
    mPinger = NULL;
  }

  ListScreen *screen = static_cast<ListScreen *>(mScreen);

  // remove the cancel item
  ListItem *cancelItem = screen->items()->pop();
  if (cancelItem != NULL) {
    delete cancelItem;
    cancelItem = NULL;
  }

  // add the rescan item
  ListItem *rescanItem = new ListItem("restart scan", RESCAN, true);
  screen->addItem(rescanItem);

  mScanStatus = FINISHED;
}

void ScanHostsProcess::pingerCallback(ip_addr_t ip, u16_t seqNum,
                                      bool didRespond, u32_t responseTime,
                                      void *arg) {
  ScanHostsProcess *scanner = static_cast<ScanHostsProcess *>(arg);

  if (didRespond) {
    scanner->addHost(ip, responseTime);
  }
}

Process *ScanHostsProcess::update() {
  if (mScanStatus == RUNNING) {
    ListScreen *screen = static_cast<ListScreen *>(this->mScreen);
    u16_t status = mPinger->send(mCurrentHost);
    if (status != 65535) {
      ip_addr_t nextIp;
      nextIp.addr = mCurrentHost.addr;
      // get the next ip that is not our own
      do {
        nextIp.addr = htonl(ntohl(nextIp.addr) + 1);
      } while (nextIp.addr == mHost.addr);

      if (ntohl(nextIp.addr) > ntohl(mEndAddress.addr)) {
        this->stopScan();
      }

      mCurrentHost = nextIp;
      char str[16];
      sprintf(str, IPSTR, IP2STR(&mCurrentHost));
      Serial.println(str);

      // calculate the new progress and show it as extra on the cancel item
      ListItem *cancelItem = screen->items()->pop();

      int hostToScan = ntohl(mEndAddress.addr) - ntohl(mStartAddress.addr);
      int hostsScanned = ntohl(mCurrentHost.addr) - ntohl(mStartAddress.addr);
      int scanProgress = (hostsScanned * 100) / hostToScan;
      String scanProgressString = String(scanProgress) + "%";

      if (cancelItem != NULL) {
        ListItem *newCancelItem =
            new ListItem(cancelItem->getName(), cancelItem->getReference(),
                         cancelItem->isClickable(), scanProgressString);
        screen->addItem(newCancelItem);
        delete cancelItem;
      }
    }
  }

  mScreen->draw();
}

void ScanHostsProcess::addHost(ip_addr_t ip, u32_t responseTime) {

  ListScreen *screen = static_cast<ListScreen *>(this->mScreen);

  ListItem *selectedItem = screen->getSelectedItem();

  char ipStr[16];
  sprintf(ipStr, IPSTR, IP2STR(&ip));
  String responseTimeStr = String(responseTime) + "ms";
  ListItem *item = new ListItem(ipStr, ipStr, true, responseTimeStr);

  ListItem *cancelItem = screen->items()->pop();
  screen->addItem(item);
  screen->addItem(cancelItem);

  bool autoScroll = selectedItem == cancelItem;
  if (autoScroll) {
    screen->cursorDown(1);
  }
}

void ScanHostsProcess::handleInput(button_t button, status_t action) {
  // make sure we're displaying a ListScreen before casting
  if (mScanStatus == NOT_STARTED) {
    if (action == Clicked && button == Left) {
      startScan();
    }
  } else if (mScanStatus == RUNNING || mScanStatus == FINISHED) {
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
        if (selectedItem->getReference() == CANCEL) {
          this->stopScan();
        } else if (selectedItem->getReference() == RESCAN) {
          this->startScan();
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
