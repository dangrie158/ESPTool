#ifndef SCAN_HOSTS_PROCESS_H_
#define SCAN_HOSTS_PROCESS_H_

#include "IPAddress.h"
#include "Pinger.h"
#include "Process.h"
#include "ScanProcess.h"

// Name of the Process used in all Screens
#define SCAN_HOST_PROCESS_NAME "Scan for Hosts"

// Identifier for the ListItem to rescan for networks
#define CANCEL "-2"

typedef enum { NOT_STARTED, RUNNING, FINISHED } host_scan_status_t;

class ScanHostsProcess : public Process {
private:
  host_scan_status_t mScanStatus;
  ip_addr_t mNetAddress;
  ip_addr_t mStartAddress;
  ip_addr_t mEndAddress;
  ip_addr_t mNetMask;
  ip_addr_t mHost;
  ip_addr_t mCurrentHost;
  SSD1306 *mDisplay;
  Pinger *mPinger;

  inline ip_addr_t IPAddress2ip_addr_t(IPAddress ip) {
    ip_addr_t retVal;
    IP4_ADDR(&retVal, ip[0], ip[1], ip[2], ip[3]);
    return retVal;
  };
  static void pingerCallback(ip_addr_t ip, u16_t seqNum, bool didRespond,
                             u32_t responseTime, void *arg);

public:
  ScanHostsProcess(SSD1306 *display);
  ~ScanHostsProcess();
  void initialize();
  Process *update();
  virtual void handleInput(button_t button, status_t action);
  void startScan();
  void stopScan();
  void showStartScreen();
  void addHost(ip_addr_t ip, u32_t responseTime);
};

#endif // SCAN_HOSTS_PROCESS_H_
