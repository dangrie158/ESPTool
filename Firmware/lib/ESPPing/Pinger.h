#ifndef __PING_H__
#define __PING_H__

#include "lwip/opt.h"

#include "lwip/icmp.h"
#include "lwip/inet_chksum.h"
#include "lwip/mem.h"
#include "lwip/netif.h"
#include "lwip/raw.h"
#include "lwip/sys.h"
#include "lwip/timers.h"

// the timeout for a single ping request
#define PING_DELAY 1000

#define PING_DATA_SIZE 32

class Pinger;

typedef struct {
  u16_t id;
  u16_t seqNum;
  Pinger *pinger;
} ping_id_t;

class Pinger {
private:
  struct raw_pcb *mPingPcb;
  ip_addr_t mIpAddr;
  u16_t mPingSeqNum;
  void (*mCallback)(u16_t seqNum, bool didRespond);
  bool *mCurrentPings;
  u16_t mMaxSimPings;
  sys_mutex_t mMutex;
  u16_t mCurrentId;

  static u8_t ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p,
                        ip_addr_t *addr);
  static void ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len,
                                u16_t seqNo, u16_t id);
  static void ping_timeout(void *arg);

public:
  Pinger(ip_addr_t ipAddr, u16_t maxSimultaneousPings = 1);
  Pinger(u8_t ip1, u8_t ip2, u8_t ip3, u8_t ip4,
         u16_t maxSimultaneousPings = 1);
  ~Pinger();
  err_t initialize();
  u16_t send();
  inline void setCallback(void (*cb)(u16_t seqNum, bool receivedPong)) {
    mCallback = cb;
  }
};

#endif /* __PING_H__ */
