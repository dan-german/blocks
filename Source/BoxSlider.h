#pragma once

#include "BoxSliderLooksAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "ThemeListener.h"

using namespace juce;

class BoxSlider: public juce::Component, juce::Slider::Listener, ThemeListener {
public:
  enum Type { tString, tFloat };

  BoxSlider();
  ~BoxSlider() override;

  void themeChanged(Theme theme) override;
  void resized() override;
  float value = 0.0f;
  float sensitivity = 0.010f;
  juce::String suffix = "";
  juce::StringArray choices;
  Slider slider;
  Label valueLabel;
private:
  BoxSliderLooksAndFeel lnf;
  void setupLabel();
  void sliderValueChanged(Slider* slider) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoxSlider)
};
