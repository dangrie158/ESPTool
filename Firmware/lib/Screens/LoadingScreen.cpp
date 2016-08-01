#include "LoadingScreen.h"

LoadingScreen::LoadingScreen(SSD1306 *display, const String message)
    : Screen(display), mMessage(message), mProgress(0), mLastUpdateTime(0),
      mLastIndeterminateProgress(0) {}

LoadingScreen::~LoadingScreen() {}

void LoadingScreen::draw() {
  this->clear();
  this->mDisplay->setColor(WHITE);
  this->mDisplay->setFont(ArialMT_Plain_10);
  this->mDisplay->setTextAlignment(TEXT_ALIGN_CENTER);

  int progress = mProgress;

  if (mProgress == INDETERMINATE) {
    long updateTime = millis();
    if (updateTime - mLastUpdateTime >= 10) {
      mLastIndeterminateProgress += 1;
      if (mLastIndeterminateProgress > 100) {
        mLastIndeterminateProgress = 0;
      }
      mLastUpdateTime = updateTime;
    }
    progress = mLastIndeterminateProgress;
  }

  this->mDisplay->drawProgressBar(0, SCREEN_HEIGHT / 2, SCREEN_WIDTH - 1, 8,
                                  progress);
  this->mDisplay->drawString(SCREEN_WIDTH / 2, (SCREEN_HEIGHT / 2) + 10,
                             mMessage);
}
