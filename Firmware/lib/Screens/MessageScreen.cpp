#include "MessageScreen.h"

MessageScreen::MessageScreen(SSD1306 *display, const String message,
                             const String ackMessage)
    : Screen(display), mMessage(message), mAckMessage(ackMessage) {}

MessageScreen::~MessageScreen() {}

void MessageScreen::draw() {
  this->clear();
  this->mDisplay->setColor(WHITE);
  this->mDisplay->setFont(ArialMT_Plain_10);
  this->mDisplay->setTextAlignment(TEXT_ALIGN_CENTER_BOTH);

  this->mDisplay->drawString(SCREEN_WIDTH / 2, (DISPLAY_HEIGHT / 2) - 12,
                             mMessage);

  this->mDisplay->setTextAlignment(TEXT_ALIGN_CENTER);
  this->mDisplay->drawString(
      SCREEN_WIDTH / 2, (SCREEN_START_Y + SCREEN_HEIGHT) - 12, mAckMessage);

  this->mDisplay->setColor(INVERSE);
  this->mDisplay->fillRect(
      SCREEN_START_X, (SCREEN_START_Y + SCREEN_HEIGHT) - 12, SCREEN_WIDTH, 12);
}
