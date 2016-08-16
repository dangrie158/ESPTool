#include "Pinger.h"
Pinger::Pinger(u16_t maxSimultaneousPings)
    : mPingSeqNum(0), mCallback(NULL), mMaxSimPings(maxSimultaneousPings),
      mCurrentPings(new LinkedList<ping_id_t *>), mCurrentId(0),
      mCallbackArg(NULL) {}

Pinger::~Pinger() {
  if (mPingPcb != NULL) {
    raw_remove(mPingPcb);
  }

  int i = mCurrentPings->size();
  while (i--) {
    ping_id_t *ping = mCurrentPings->pop();
    sys_untimeout(Pinger::ping_timeout, ping);
    delete ping;
  }

  // delete mCurrentPings;
  sys_mutex_free(&mMutex);
}

err_t Pinger::initialize() {
  err_t error = ERR_OK;

  // create a new raw ip control block
  mPingPcb = raw_new(IP_PROTO_ICMP);

  // set the receive listener for the block
  raw_recv(mPingPcb, Pinger::ping_recv, this);
  // bin the pcb to all ip addresses
  error = raw_bind(mPingPcb, IP_ADDR_ANY);

  // create a new mutex to avoid race-conditions
  // while checking if a response was received in time
  sys_mutex_new(&mMutex);
  return error;
}

u8_t Pinger::ping_recv(void *arg, struct raw_pcb *pcb, struct pbuf *p,
                       ip_addr_t *addr) {
  struct icmp_echo_hdr *iecho;
  u32_t recvTime = sys_now();

  // the Pinger is set as recv_arg while registering the receive callback
  Pinger *self = static_cast<Pinger *>(pcb->recv_arg);

  LWIP_UNUSED_ARG(pcb);
  LWIP_UNUSED_ARG(addr);

  if (pbuf_header(p, -PBUF_IP_HLEN) == 0) {
    // get the echo response payload
    iecho = (struct icmp_echo_hdr *)p->payload;

    // get the ID
    u16_t echoId = ntohs(iecho->id);

    // get the sequence number
    u16_t seqNo = ntohs(iecho->seqno);

    // lock the mutex
    sys_mutex_lock(&mMutex);

    // check if we have an outstanding response for this sequence number
    ping_id_t *ping = self->getPingBySeqNo(seqNo);
    if (ping != NULL) {

      ip_addr_t ip = ping->ip;
      u32_t responseTime = recvTime - ping->pingTime;

      // call the callback
      if (self->mCallback != NULL) {
        self->mCallback(ip, seqNo, true, responseTime, self->mCallbackArg);
      }

      // clear the timeout
      sys_untimeout(Pinger::ping_timeout, ping);
      // clean up for this ping
      self->mCurrentPings->remove(ping);
      delete ping;
    }
    // unock the mutex
    sys_mutex_unlock(&mMutex);
    // free the packet buffer
    pbuf_free(p);

    // eat the packet
    return 1;
  }

  // don't eat the packet
  return 0;
}

ping_id_t *Pinger::getPingBySeqNo(u16_t seqNo) {
  u16_t i = 0;
  ping_id_t *currentPing = this->mCurrentPings->at(i);

  while (currentPing != NULL) {
    if (currentPing->seqNum == seqNo) {
      return currentPing;
    }
    currentPing = this->mCurrentPings->at(++i);
  }
  return NULL;
}

ping_id_t *Pinger::getPingById(u16_t id) {
  u16_t i = 0;
  ping_id_t *currentPing = this->mCurrentPings->at(i);

  while (currentPing != NULL) {
    if (currentPing->id == id) {
      return currentPing;
    }
    currentPing = this->mCurrentPings->at(++i);
  }
  return NULL;
}

void Pinger::ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len,
                               u16_t seqNo) {
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  // set all the required fields
  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id = PING_ID;
  iecho->seqno = htons(seqNo);

  // fill the additional data buffer with some data
  for (i = 0; i < data_len; i++) {
    ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  // calculate and set the checksum
  iecho->chksum = inet_chksum(iecho, len);
}

u16_t Pinger::send(ip_addr_t ip) {
  struct icmp_echo_hdr *iecho;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

  u16_t seqNo = -1;

  // calculate the next sequence number
  u16_t nextSeqNo = this->mPingSeqNum + 1;

  u16_t nextId = this->mCurrentId;
  int freeSlots = this->mMaxSimPings;
  ping_id_t *ping = NULL;
  do {
    nextId += 1;
    nextId %= mMaxSimPings;
    ping = this->getPingById(nextId);
  } while (ping != NULL && --freeSlots > 0);

  // if the sequence number is currently in use, we have no more free pings
  // available, another ping has to respond or timeout first.
  if (ping != NULL) {
    return -1;
  }

  sys_mutex_lock(&mMutex);

  // allocate a new packet buffer
  struct pbuf *p;
  p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (!p) {
    pbuf_free(p);
    return -1;
  }
  if ((p->len == p->tot_len) && (p->next == NULL)) {

    // create a new ping_id_t for the timeout callback to pass
    ping_id_t *id = new ping_id_t;
    id->pinger = this;
    id->id = nextId;
    id->seqNum = nextSeqNo;
    id->ip = ip;
    this->mCurrentId = nextId;

    // mark the current sequence number as not available
    mCurrentPings->push_back(id);

    sys_mutex_unlock(&mMutex);

    // increment the sequence number
    this->mPingSeqNum = nextSeqNo;

    // get the data field of the ip packet
    iecho = (struct icmp_echo_hdr *)p->payload;

    // prepare the ICMP Echo Request
    Pinger::ping_prepare_echo(iecho, (u16_t)ping_size, nextSeqNo);

    // register the timeout
    sys_timeout(PING_DELAY, ping_timeout, id);

    // send the request
    id->pingTime = sys_now();
    raw_sendto(mPingPcb, p, &ip);

    // set the return value;
    seqNo = this->mPingSeqNum;

    // free the packet buffer
  }
  pbuf_free(p);

  return seqNo;
}

void Pinger::ping_timeout(void *arg) {
  // get the ping_id_t that was specified as argument
  ping_id_t *id = static_cast<ping_id_t *>(arg);

  Pinger *pinger = id->pinger;
  if (pinger == NULL) {
    // delete id;
    return;
  }
  u16_t seqNo = id->seqNum;
  ip_addr_t ip = id->ip;

  // lock the mutex before checking the state
  sys_mutex_lock(&mMutex);
  ping_id_t *ping = pinger->getPingBySeqNo(seqNo);
  if (ping != NULL) {

    // call the callback
    if (pinger->mCallback != NULL) {
      pinger->mCallback(ip, seqNo, false, -1, pinger->mCallbackArg);
    }

    // free the state of the current sequence number
    pinger->mCurrentPings->remove(ping);
    delete ping;
  }

  // free the mutex again
  sys_mutex_unlock(&mMutex);
}
