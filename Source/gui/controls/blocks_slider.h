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

class BlocksSlider: public BaseButton, juce::Slider::Listener, ThemeListener {
public:
  class Listener;
  enum Type { tString, tFloat };
  Colour current;
  float value = 0.0f;
  float sensitivity = 0.010f;
  juce::String suffix = "";
  juce::StringArray choices;
  Slider juce_slider_;
  Label value_label_;
  float default_value_ = 0.0f;

  bool modulatable = true;
  ID module_id_;
  std::string parameter_name_;
  Component slider_container_;
  bool custom_track_colour_ = false;

  BlocksSlider(Listener* listener);
  ~BlocksSlider() override;

  void sliderDragStarted(Slider* slider) override;
  void sliderDragEnded(Slider* slider) override;
  void setupSliderContainer();
  void setupSlider();
  void setupIndicationAnimator();

  void themeChanged(Theme theme) override;
  void resized() override;
  void resizeSelectionHighlight();
  void setIndicationHighlight(bool shouldHighlight, Colour color);

  void setButtonColour(Colour colour) override {};
  Component* getContent() override { return &slider_container_; }
  void startModulationSelectionAnimation();
  void stopModulationSelectionAnimation();
  inline void addListener(Listener* listener) { listeners_.push_back(listener); }
protected:
  void selectedAnimation(float value, float progress) override;
  void deselectedAnimation(float value, float progress) override;
  void selectedCompletion() override;
  void deselectedCompletion() override;
  void mouseDown(const MouseEvent& event) override;
private:
  std::vector<Listener*> listeners_;
  ValueAnimator value_animator_;
  DrawablePath modulation_indication_highlight_;
  DrawablePath modulation_selection_highlight_;
  bool is_mouse_inside_ = false;

  void mouseEnter(const MouseEvent& event) override;
  void mouseExit(const MouseEvent& event) override;
  BoxSliderLooksAndFeel lnf;
  void setupLabel();
  void sliderValueChanged(Slider* slider) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlocksSlider)
};

struct BlocksSlider::Listener {
  virtual void sliderAdjusted(BlocksSlider* slider, float value) = 0;
  virtual void sliderGestureChanged(BlocksSlider* slider, bool started) {};
};
