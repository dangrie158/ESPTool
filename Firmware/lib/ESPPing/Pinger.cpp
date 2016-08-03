#include "Pinger.h"
Pinger::Pinger(ip_addr_t ipAddr, u16_t maxSimultaneousPings)
    : mIpAddr(ipAddr), mPingSeqNum(0), mCallback(NULL),
      mMaxSimPings(maxSimultaneousPings),
      mCurrentPings(new bool[maxSimultaneousPings]), mCurrentId(0) {}

Pinger::Pinger(u8_t ip1, u8_t ip2, u8_t ip3, u8_t ip4,
               u16_t maxSimultaneousPings)
    : mPingSeqNum(0), mCallback(NULL), mMaxSimPings(maxSimultaneousPings),
      mCurrentPings(new bool[maxSimultaneousPings]), mCurrentId(0) {
  IP4_ADDR(&mIpAddr, ip1, ip2, ip3, ip4);
}

Pinger::~Pinger() {
  delete[] mCurrentPings;
  sys_mutex_free(&mMutex);
}

err_t Pinger::initialize() {
  err_t error = ERR_OK;
  // 0 initialize all values
  for (int i = 0; i < mMaxSimPings; i++) {
    mCurrentPings[i] = false;
  }

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
    if (self->mCurrentPings[echoId] != false) {

      // call the callback
      if (self->mCallback != NULL) {
        self->mCallback(seqNo, true);
      }

      // reset the state for this sequence number to avoid the timeout from
      // calling back
      self->mCurrentPings[echoId] = false;
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

void Pinger::ping_prepare_echo(struct icmp_echo_hdr *iecho, u16_t len,
                               u16_t seqNo, u16_t id) {
  size_t i;
  size_t data_len = len - sizeof(struct icmp_echo_hdr);

  // set all the required fields
  ICMPH_TYPE_SET(iecho, ICMP_ECHO);
  ICMPH_CODE_SET(iecho, 0);
  iecho->chksum = 0;
  iecho->id = id;
  iecho->seqno = htons(seqNo);

  // fill the additional data buffer with some data
  for (i = 0; i < data_len; i++) {
    ((char *)iecho)[sizeof(struct icmp_echo_hdr) + i] = (char)i;
  }

  // calculate and set the checksum
  iecho->chksum = inet_chksum(iecho, len);
}

u16_t Pinger::send() {
  struct pbuf *p;
  struct icmp_echo_hdr *iecho;
  size_t ping_size = sizeof(struct icmp_echo_hdr) + PING_DATA_SIZE;

  // allocate a new packet buffer
  p = pbuf_alloc(PBUF_IP, (u16_t)ping_size, PBUF_RAM);
  if (!p) {
    return -1;
  }

  u16_t seqNo = -1;

  if ((p->len == p->tot_len) && (p->next == NULL)) {

    // calculate the next sequence number
    u16_t nextId = this->mCurrentId + 1;
    nextId %= mMaxSimPings;

    // if the sequence number is currently in use, we have no more free pings
    // available, another ping has to respond or timeout first.
    sys_mutex_lock(&mMutex);
    if (mCurrentPings[nextId] == true) {
      pbuf_free(p);
      return -1;
    }

    // mark the current sequence number as not available
    mCurrentPings[nextId] = true;

    sys_mutex_unlock(&mMutex);

    // set the next sequence number
    this->mCurrentId = nextId;

    // increment the sequence number
    u16_t seqNo = ++this->mPingSeqNum;

    // get the data field of the ip packet
    iecho = (struct icmp_echo_hdr *)p->payload;

    // prepare the ICMP Echo Request
    Pinger::ping_prepare_echo(iecho, (u16_t)ping_size, seqNo, nextId);

    // send the request
    raw_sendto(mPingPcb, p, &mIpAddr);

    // create a new ping_id_t for the timeout callback to pass
    ping_id_t *id = new ping_id_t;
    id->pinger = this;
    id->id = nextId;
    id->seqNum = seqNo;

    // register the timeout
    sys_timeout(PING_DELAY, ping_timeout, id);

    // set the return value;
    seqNo = this->mPingSeqNum;
  }

  // free the packet buffer
  pbuf_free(p);

  return seqNo;
}

void Pinger::ping_timeout(void *arg) {
  // get the ping_id_t that was specified as argument
  ping_id_t *id = static_cast<ping_id_t *>(arg);

  Pinger *pinger = id->pinger;
  u16_t idNum = id->id;
  u16_t seqNo = id->seqNum;

  // lock the mutex before checking the state
  sys_mutex_lock(&mMutex);
  if (pinger->mCurrentPings[idNum] == true) {

    // call the callback
    if (pinger->mCallback != NULL) {
      pinger->mCallback(seqNo, false);
    }

    // free the state of the current sequence number
    pinger->mCurrentPings[idNum] = false;
  }

  // free the mutex again
  sys_mutex_unlock(&mMutex);

  // free the memry of the ping_id_t
  delete id;
}
