#pragma once

#include "gui/BoxSliderLooksAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/ThemeListener.h"
#include "gui/ValueAnimator.h"
#include "gui/EasingAnimator.h"
#include "gui/HighlightComponent.h"
#include "model/id.h"
#include "gui/base/BaseButton.h"

using namespace juce;

class BoxSlider: public BaseButton, juce::Slider::Listener, ThemeListener {
public:
  enum Type { tString, tFloat };

  BoxSlider();
  void setupIndicationAnimator();
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
  void setIndicationHighlight(bool shouldHighlight, Colour color);
  bool modulatable = true;
  ID module_id_;
  std::string parameter_name_;
  void setButtonColour(Colour colour) override {};
  Component slider_parent;

  Component* getContent() override { return &slider_parent; }
protected:
  void selectedAnimation(float value, float progress) override;
  void deselectedAnimation(float value, float progress) override;
  void selectedCompletion() override;
  void deselectedCompletion() override;
  void mouseDown(const MouseEvent& event) override;
private:
  ValueAnimator value_animator_;
  DrawablePath modulation_indication_highlight_;
  DrawableRectangle modulation_selection_highlight_;
  EasingAnimator easing_animator_;
  bool is_mouse_inside_ = false;

  void mouseEnter(const MouseEvent& event) override;
  void mouseExit(const MouseEvent& event) override;
  BoxSliderLooksAndFeel lnf;
  void setupLabel();
  void sliderValueChanged(Slider* slider) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoxSlider)
};
