#pragma once

#include "gui/BoxSliderLooksAndFeel.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/ThemeListener.h"
#include "gui/ValueAnimator.h"
#include "gui/HighlightComponent.h"
#include "model/id.h"
#include "gui/base/BaseButton.h"
#include "gui/modulation_indicator.h"

using namespace juce;

class BoxSlider: public BaseButton, juce::Slider::Listener, ThemeListener {
public:
  enum Type { tString, tFloat };
  Colour current;
  float value = 0.0f;
  float sensitivity = 0.010f;
  juce::String suffix = "";
  juce::StringArray choices;
  Slider slider_;
  Label value_label_;
  float default_value_ = 0.0f;

  bool modulatable = true;
  ID module_id_;
  std::string parameter_name_;
  Component slider_container_;

  BoxSlider();
  void setupSliderContainer();
  void setupSlider();
  void setupIndicationAnimator();
  ~BoxSlider() override;

  void themeChanged(Theme theme) override;
  void resized() override;
  void resizeSelectionHighlight();
  void setIndicationHighlight(bool shouldHighlight, Colour color);

  void setButtonColour(Colour colour) override {};
  Component* getContent() override { return &slider_container_; }
  void startModulationSelectionAnimation();
  void stopModulationSelectionAnimation();
protected:
  // void startSelectedAnimation() override;
  void selectedAnimation(float value, float progress) override;
  void deselectedAnimation(float value, float progress) override;
  void selectedCompletion() override;
  void deselectedCompletion() override;
  void mouseDown(const MouseEvent& event) override;
private:
  ValueAnimator value_animator_;
  DrawablePath modulation_indication_highlight_;
  DrawablePath modulation_selection_highlight_;
  bool is_mouse_inside_ = false;

  void mouseEnter(const MouseEvent& event) override;
  void mouseExit(const MouseEvent& event) override;
  BoxSliderLooksAndFeel lnf;
  void setupLabel();
  void sliderValueChanged(Slider* slider) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BoxSlider)
};
