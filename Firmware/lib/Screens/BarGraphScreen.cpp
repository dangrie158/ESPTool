#include "BarGraphScreen.h"

BarGraphScreen::BarGraphScreen(SSD1306 *display, const String message,
                               int numBars)
    : Screen(display), mNumBars(numBars), mBarValues(new int[numBars]),
      mBarTitles(new String[numBars]), mMessage(message) {
  for (int i = 0; i < numBars; i++) {
    mBarValues[i] = 0;
    mBarTitles[i] = "";
  }
}

BarGraphScreen::~BarGraphScreen() {
  delete[] mBarTitles;
  delete[] mBarValues;
}

void BarGraphScreen::draw() {
  this->clear();
  mDisplay->setFont(ArialMT_Plain_10);
  mDisplay->setColor(WHITE);
  mDisplay->setTextAlignment(TEXT_ALIGN_CENTER);

  int screenHeightLeft = SCREEN_HEIGHT;
  // draw the title if any
  if (mMessage.length() > 0) {
    mDisplay->drawString(SCREEN_WIDTH / 2, SCREEN_START_Y, mMessage);
    screenHeightLeft -= 12;
  }

  // while iterating over the labels, use the chance to find the biggest bar
  // value
  int maxBarValue = 0;

  // draw the bar labels
  float barWidth = (SCREEN_WIDTH / (float)mNumBars);
  int labelStartY = (SCREEN_START_Y + SCREEN_HEIGHT) - 10;
  for (int i = 0; i < mNumBars; i++) {
    int labelCenterX = (barWidth * i) + (barWidth / 2);
    mDisplay->drawString(labelCenterX, labelStartY, mBarTitles[i]);

    // while iterating over the labels, use the chance to find the biggest bar
    // value, otherwise we would need to iterate one more time just for this
    if (mBarValues[i] > maxBarValue) {
      maxBarValue = mBarValues[i];
    }
  }
  screenHeightLeft -= 12;

  // draw the bars

  // the height of a bar with value 1
  // use flot or we could accumulate the error and end up with an error of
  // maxBarValue / 2 pixels
  float unitHeight = screenHeightLeft / (float)maxBarValue;
  for (int i = 0; i < mNumBars; i++) {
    mDisplay->setColor(WHITE);
    int barValue = mBarValues[i];
    int barHeight = max(1, unitHeight * barValue);
    int barStartX = (barWidth * i) + 1;
    int barStartY = (SCREEN_START_Y + SCREEN_HEIGHT - 10) - barHeight;
    mDisplay->fillRect(barStartX, barStartY, barWidth - 2, barHeight);

    // draw the labels inside of the bars
    if (barValue > 0) {
      mDisplay->setColor(INVERSE);
      int labelCenterX = (barWidth * i) + (barWidth / 2);
      int labelStartY = min(barStartY, barStartY + barHeight - 10);
      mDisplay->drawString(labelCenterX, labelStartY, String(barValue));
    }
  }
}
