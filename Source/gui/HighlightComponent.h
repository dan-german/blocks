/*
  ==============================================================================

    HighlightComponent.h
    Created: 8 Mar 2021 2:37:51pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/ValueAnimator.h"
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class HighlightComponent: public juce::Component {
public:
  Colour colour;
  ValueAnimator valueAnimator;
  HighlightComponent();
  ~HighlightComponent() override;
  void paint(juce::Graphics&) override;
  void resized() override;
private:
  float y = 0.0f;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HighlightComponent)
};
