#include <juce_gui_basics/juce_gui_basics.h>
#include "CircleIndicator.h"

using namespace juce;

class CircleIndicatorList: public Component, public ListBoxModel {
public:
  ListBox listBox;

  CircleIndicatorList() {
    addAndMakeVisible(listBox);
    listBox.setColour(ListBox::ColourIds::backgroundColourId, Colours::transparentBlack);
    listBox.setModel(this);
  }

  void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override {};
  int getNumRows() override { return 4; };

  void resized() override {
    listBox.setBounds(getLocalBounds());
    listBox.setRowHeight(getWidth());
  }

  Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override {
    if (existingComponentToUpdate) return existingComponentToUpdate;
    return new CircleIndicator();
  };
};