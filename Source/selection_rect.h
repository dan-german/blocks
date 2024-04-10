#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/controls/BoxSlider.h"
#include "gui/ThemeListener.h"

class SelectionRect: public juce::Component, ThemeListener {
public:
  class Listener;
  Listener* listener;

  SelectionRect();
  ~SelectionRect();
  void paint(juce::Graphics&) override;
  void resized() override;
  void reset();
private:
  Path stroke_;
  void themeChanged(Theme theme) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SelectionRect)
};