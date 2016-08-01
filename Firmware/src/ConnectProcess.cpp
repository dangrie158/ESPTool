#include "ConnectProcess.h"
#include "ESP8266WiFi.h"
#include "ListScreen.h"
#include "LoadingScreen.h"
#include "Process.h"
#include "TextInputScreen.h"

ConnectProcess::ConnectProcess(SSD1306 *display, int wlanNumberFromScan)
    : Process(CONNECT_PROCESS_NAME, NULL),
      mWlanNumberFromScan(wlanNumberFromScan), mCurrentStatus(UNKNOWN),
      mDisplay(display) {
  this->initialize();
}

void ConnectProcess::initialize() {
  if (WiFi.encryptionType(mWlanNumberFromScan) != ENC_TYPE_NONE) {
    promptForPassword();
  } else {
    connectToWiFi("");
  }
}

void ConnectProcess::promptForPassword() {
  String screenPrompt = PROMPT_MESSAGE;
  screenPrompt += String('\n');
  screenPrompt += WiFi.SSID(mWlanNumberFromScan);

  Screen *tempScreen = this->mScreen;
  Screen *txtInScreen = new TextInputScreen(mDisplay, screenPrompt);
  this->setScreen(txtInScreen);

  if (tempScreen != NULL) {
    delete tempScreen;
  }

  mCurrentStatus = PROMPTING;
}

void ConnectProcess::connectToWiFi(const String &passphrase) {
  String screenPrompt = CONNECTING_MESSAGE;
  String ssid = WiFi.SSID(mWlanNumberFromScan);

  screenPrompt += String('\n');
  screenPrompt += ssid;

  Screen *tempScreen = this->mScreen;
  LoadingScreen *connectingScreen = new LoadingScreen(mDisplay, screenPrompt);
  connectingScreen->setProgress(INDETERMINATE);
  this->setScreen(connectingScreen);

  if (tempScreen != NULL) {
    delete tempScreen;
  }
  Serial.println(ssid.c_str());
  Serial.println(passphrase.c_str());
  WiFi.begin(ssid.c_str(), passphrase.c_str());
  mCurrentStatus = CONNECTING;
}

void ConnectProcess::showConnectedSummary() {
  String ssid = WiFi.SSID(mWlanNumberFromScan);
  String localIp = ipToString(WiFi.localIP());
  String gatewayIp = ipToString(WiFi.gatewayIP());
  String subnetMask = ipToString(WiFi.subnetMask());
  String macAddress = WiFi.macAddress();

  Screen *tempScreen = this->mScreen;
  ListScreen *connectSummaryScreen = new ListScreen(mDisplay, false);

  connectSummaryScreen->addItem(new ListItem("Connected", "", false, ssid));
  connectSummaryScreen->addItem(new ListItem("IP:", "", false, localIp));
  connectSummaryScreen->addItem(new ListItem("Gateway:", "", false, gatewayIp));
  connectSummaryScreen->addItem(new ListItem("Subnet:", "", false, subnetMask));
  connectSummaryScreen->addItem(new ListItem("Mac:", "", false, macAddress));

  this->setScreen(connectSummaryScreen);

  if (tempScreen != NULL) {
    delete tempScreen;
  }
  mCurrentStatus = CONNECTED;
}

ConnectProcess::~ConnectProcess() { delete mScreen; }

Process *ConnectProcess::update() {
  if (mCurrentStatus == CONNECTING && WiFi.status() == WL_CONNECTED) {
    // we are connected now, show the
    showConnectedSummary();
  }
  mScreen->draw();
}

void ConnectProcess::handleInput(button_t button, status_t action) {

  if (action == Released || action == Open) {
    mDidDelete = false;
    return;
  }
  if (mCurrentStatus == PROMPTING) {
    TextInputScreen *screen = static_cast<TextInputScreen *>(this->mScreen);
    if (action == Clicked) {
      switch (button) {
      case Up:
        screen->charUp();
        break;
      case Down:
        screen->charDown();
        break;
      case Left:
        screen->nextChar();
        break;
      }
    }

    if (action == Held) {
      switch (button) {
      case Up:
        screen->charUp();
        break;
      case Down:
        screen->charDown();
        break;
      case Left:
        String passphrase = screen->getInputString();
        connectToWiFi(passphrase);
        break;
      }
    }

    if (action == DoubleClicked && button == Left) {
      if (!mDidDelete) {
        screen->prevChar();
        mDidDelete = true;
      }
    }
  } else if (mCurrentStatus == CONNECTED) {
    ListScreen *screen = static_cast<ListScreen *>(this->mScreen);
    if (action == Clicked) {
      switch (button) {
      case Up:
        screen->cursorUp(1);
        break;
      case Down:
        screen->cursorDown(1);
        break;
      case Left:
        // the process has finished, call back

        break;
      }
    }
  }
}
