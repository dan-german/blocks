#pragma once
#include "gui/controls/SVGButton.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "BinaryData.h"
#include "gui/ThemeListener.h"

using namespace juce;

class PresetButtonContent: public Component, public ThemeListener {
public:
  PresetButtonContent();
  ~PresetButtonContent() override;
  Label label;
  Label left_arrow_;
  Label right;
  void paint(juce::Graphics&) override;
  void resized() override;
  void setupLabel(Label& label, String text, Colour color, float fontSize);
  void themeChanged(Theme theme) override;
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetButtonContent)
};