#include <Arduino.h>
#include <Ticker.h>
#include <Wire.h>

#include "AnalogInput.h"
#include "Button.h"
#include "SSD1306.h"
#include "ScanProcess.h"

// Include custom images
#include "images.h"

// include pin definitions
#include "defs.h"

SSD1306 display(0x3C, 4, 5);
AnalogInput batteryVoltage(VBAT_PIN, VBAT_MAX / ADC_MAX, 10);
Button down(12, true, true);
Button up(13, true, true);
Button left(14, true, true);
Ticker buttonUpdater;

ScanProcess scanner(&display);

Process *currentProcess = &scanner;

void updateButton() {
  down.update();
  up.update();
  left.update();
}

void drawStatusBar() {
  // draw the battery indicator and voltage
  float currentVoltage = batteryVoltage.readConverted();
  int screenWidthLeft = DISPLAY_WIDTH;

  const char *currentVoltageImage = NULL;
  if (currentVoltage >= 3.6) {
    currentVoltageImage = icon_battery_33;
  } else if (currentVoltage >= 3.4) {
    currentVoltageImage = icon_battery_23;
  } else if (currentVoltage >= 3.2) {
    currentVoltageImage = icon_battery_13;
  } else {
    currentVoltageImage = icon_battery_03;
  }

  display.drawXbm(DISPLAY_WIDTH - ICON_BATTERY_WIDTH, 2, ICON_BATTERY_WIDTH,
                  ICON_BATTERY_HEIGHT, currentVoltageImage);

  screenWidthLeft -= ICON_BATTERY_WIDTH + 2;

  display.setTextAlignment(TEXT_ALIGN_RIGHT);
  display.setFont(ArialMT_Plain_10);
  String voltageText = String(currentVoltage, 2) + "V";
  display.drawString(screenWidthLeft, 0, voltageText);
  display.drawLine(0, 11, DISPLAY_WIDTH, 11);

  screenWidthLeft -= display.getStringWidth(voltageText) + 4;

  // draw the sdcard icon if a card is present
  display.drawXbm(screenWidthLeft - ICON_SDCARD_WIDTH, 2, ICON_SDCARD_WIDTH,
                  ICON_SDCARD_HEIGHT, icon_sdcard);

  screenWidthLeft -= ICON_SDCARD_WIDTH + 4;

  // draw the current menu location
  String currentScreenName = String("ESPToolalskdjlaskdjlas");
  while (display.getStringWidth(currentScreenName) > screenWidthLeft) {
    currentScreenName.remove(currentScreenName.length() - 4);
    currentScreenName += "...";
  }

  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.drawString(0, 0, currentScreenName);
}

void setup() {
  display.init();

  display.flipScreenVertically();

  pinMode(VBAT_PIN, INPUT);

  Serial.begin(115200);
  Serial.println("lalalalala test");

  buttonUpdater.attach_ms(1, updateButton);

  currentProcess->initialize();
}

void handleInput() {
  status_t status = up.getStatus();
  if (status != Open) {
    currentProcess->handleInput(Up, status);
  }

  status = down.getStatus();
  if (status != Open) {
    currentProcess->handleInput(Down, status);
  }

  status = left.getStatus();
  if (status != Open) {
    currentProcess->handleInput(Left, status);
  }
}

void loop() {
  display.clear();
  drawStatusBar();

  currentProcess->update();

  handleInput();

  display.display();
}
