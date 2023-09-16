#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

#include "EasingAnimator.h"
using namespace juce;

class BasePopup: public juce::Component {
public:
  float cornerRadius = 12.0f;
  BasePopup() = default;

  void paint(juce::Graphics& g) override;

  virtual void setBackgroundColour(Colour newColour) {
    this->backgroundColour = newColour;
    repaint();
  }

  void setVisible(bool shouldBeVisible) override;

  void triggerDismissAnimation();

  void present();
private:
  Colour colour;
  Colour backgroundColour;
  EasingAnimator animator;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BasePopup)
};
