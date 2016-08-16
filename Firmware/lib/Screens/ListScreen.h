#ifndef _LISTSCREEN_H_
#define _LISTSCREEN_H_

#include "LinkedList.h"
#include "SSD1306.h"
#include "Screen.h"
#include <Arduino.h>

#define ITEM_HEIGHT 11
#define ITEMS_ON_SCREEN ((SCREEN_HEIGHT) / (ITEM_HEIGHT))
#define INDICATOR_WIDTH 12
#define ELEMENT_INDICATOR ">"
#define PADDING 5
#define LIST_OFFSET 3

class ListItem {
private:
  const String mName;
  const String mReference;
  const bool mClickable;
  const String mExtra;

public:
  inline ListItem(String name, String reference, bool clickable = false,
                  String extra = "")
      : mName(name), mReference(reference), mClickable(clickable),
        mExtra(extra) {}

  inline const String getName() { return mName; }
  inline const String getReference() { return mReference; }
  inline const bool isClickable() { return mClickable; }
  inline const String getExtra() { return mExtra; }
};

class ListScreen : public Screen {
private:
  LinkedList<ListItem *> *mItems;
  int mScrollOffset;
  int mSelectedElementOnScreen;
  bool mSelectable;

public:
  ListScreen(SSD1306 *display, bool elementsAreSelectable = true);
  ~ListScreen();
  void draw();
  inline void addItem(ListItem *item) { mItems->push_back(item); }

  bool cursorDown(int steps = 1);
  bool cursorUp(int steps = 1);

  inline ListItem *getSelectedItem() {
    return mItems->at(mScrollOffset + mSelectedElementOnScreen);
  }

  inline LinkedList<ListItem *> *items() { return mItems; }

  inline void setSelectable(bool isSelectable) { mSelectable = isSelectable; }
};

#endif //_LISTSCREEN_H_
