#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "model/Index.h"
#include "model/ButtonGridModel.h"

using namespace juce;

class ButtonGrid: public juce::Component {
public:
  int itemHeight;
  std::vector<std::unique_ptr<ListBox>> listBoxes;
  std::vector<std::unique_ptr<ButtonGridModel>> listBoxModels;
  std::function<void(Index)> onClick;
  Colour backgroundColour;
  int numberOfColumns = 0;
  int numberOfRows = 0;

  ButtonGrid();

  int getNumberOfColumns();
  int getNumberOfRows();
  void setModel(Array<StringArray> models);
  void resized() override;
  void present(std::function<void(Index)> callback);

private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ButtonGrid)
};
