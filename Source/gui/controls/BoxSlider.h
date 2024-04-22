#pragma once

#include "gui/BoxSliderLooksAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/ThemeListener.h"
#include "gui/ValueAnimator.h"
#include "gui/HighlightComponent.h"
#include "model/id.h"
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
  float default_value_ = 0.0f;
  void paint(juce::Graphics& g) override; 
  void highlight(bool shouldHighlight, Colour color);
  bool modulatable = true;
  ID module_id_;
  std::string parameter_name_;
private:
  ValueAnimator animator_;
  DrawablePath drawable_path_;
  void mouseDown(const MouseEvent& event) override;
  BoxSliderLooksAndFeel lnf;
  void setupLabel();
  void sliderValueChanged(Slider* slider) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoxSlider)
};
