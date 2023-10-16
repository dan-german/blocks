#include "gui/controls/ButtonGridPopup.h"
#include "gui/ThemeManager.h"

ButtonGridPopup::ButtonGridPopup() {
  addAndMakeVisible(menu);
}

int ButtonGridPopup::calculateHeight(int itemCount) {
  return itemCount * itemHeight + borderSize * 2;
}

void ButtonGridPopup::setModel(Array<StringArray> stringArrays) {
  setBackgroundColour(ThemeManager::shared()->getCurrent().background.brighter(0.3f));
  menu.setModel(stringArrays);
}

void ButtonGridPopup::resized() {
  int height = getHeight() - borderSize * 2;
  menu.itemHeight = scrollMode ? itemHeight : height / menu.getNumberOfRows();
  menu.setBounds(borderSize, borderSize, getWidth() - borderSize * 2, height);
  repaint();
}

void ButtonGridPopup::present(std::function<void(Index)> callback) {
  BasePopup::present();
  menu.present(callback);
}