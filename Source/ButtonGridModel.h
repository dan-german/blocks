#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "Index.h"
#include "LabelButton.h"

using namespace juce;

class ButtonGridModel: public ListBoxModel {
public:
  int column = 0;
  StringArray texts;
  std::function<void(Index)> onClick;

  ButtonGridModel(StringArray texts, int column);
  void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
  int getNumRows() override;
  Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
};
