#pragma once

#include "gui/controls/ButtonGrid.h"
#include "gui/base/BasePopup.h"

class ButtonGridPopup: public BasePopup {
public:
  ButtonGrid menu;
  bool scrollMode = false;
  int itemHeight = 20;
  int borderSize = 5;

  ButtonGridPopup();
  int calculateHeight(int itemCount);

  void setModel(Array<StringArray> stringArrays);
  void resized() override;
  void present(std::function<void(Index)> callback);
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonGridPopup)
};
