#pragma once
#include "gui/controls/SVGButton.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"

using namespace juce;

class PresetButtonContent: public Component {
public:
  PresetButtonContent();
  Label label;
  void paint(juce::Graphics&) override;
  void resized() override;
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetButtonContent)
};