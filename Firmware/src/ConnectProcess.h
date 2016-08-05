#ifndef CONNECT_PROCESS_H_
#define CONNECT_PROCESS_H_

#include "ESP8266WiFi.h"
#include "Process.h"

// Name of the Process used in all Screens
#define CONNECT_PROCESS_NAME "Connect to ..."

// message used on the loading screen
#define PROMPT_MESSAGE "Enter Key for"

// message displayed while connecting (the ssid is appended at runtime)
#define CONNECTING_MESSAGE "connecting to"

typedef enum {
  UNKNOWN,
  PROMPTING,
  CONNECTING,
  CONNECTED,
  FAILED
} connect_status_t;

class ConnectProcess : public Process {
private:
  bool mDidDelete;
  int mWlanNumberFromScan;
  connect_status_t mCurrentStatus;
  SSD1306 *mDisplay;

  void promptForPassword();
  void connectToWiFi(const String &passphrase);
  void showConnectedSummary();
  void showConnectionError();

public:
  ConnectProcess(SSD1306 *display, int wlanNumberFromScan);
  ~ConnectProcess();
  void initialize();
  Process *update();
  void handleInput(button_t button, status_t action);
  inline String ipToString(const IPAddress &address) {
    return String(address[0]) + "." + String(address[1]) + "." +
           String(address[2]) + "." + String(address[3]);
  }
};

#endif // CONNECT_PROCESS_H_
