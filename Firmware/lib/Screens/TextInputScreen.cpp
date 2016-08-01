#include "TextInputScreen.h"

TextInputScreen::TextInputScreen(SSD1306 *display, const String message)
    : Screen(display), mMessage(message), mInput((char)START_CHAR),
      mCurrentCharPos(0) {}

TextInputScreen::~TextInputScreen() {}

void TextInputScreen::draw() {
  this->clear();
  this->mDisplay->setColor(WHITE);
  this->mDisplay->setFont(ArialMT_Plain_10);
  this->mDisplay->setTextAlignment(TEXT_ALIGN_CENTER);

  this->mDisplay->drawString(SCREEN_WIDTH / 2, SCREEN_START_Y + 20, mMessage);

  // use a progressbar with 0 progress to easily draw a nice frame
  this->mDisplay->drawProgressBar(0, SCREEN_START_Y, SCREEN_WIDTH - 1, 16, 0);

  // draw the current input
  this->mDisplay->setTextAlignment(TEXT_ALIGN_LEFT);
  int inputStartX = 8;

  this->mDisplay->drawString(inputStartX, SCREEN_START_Y + 1, mInput);

  // draw the (blinking) cursor (0.5Hz)
  if ((millis() % 1000) > 500) {
    this->mDisplay->setColor(INVERSE);
    String inputToStartCurrentChar = mInput.substring(0, mCurrentCharPos);
    String inputToEndCurrentChar = mInput.substring(0, mCurrentCharPos + 1);
    int cursorStartX =
        inputStartX + this->mDisplay->getStringWidth(inputToStartCurrentChar);
    int cursorEndX =
        inputStartX + this->mDisplay->getStringWidth(inputToEndCurrentChar);
    this->mDisplay->fillRect(cursorStartX, SCREEN_START_Y + 3,
                             cursorEndX - cursorStartX, 12);
  }
}
