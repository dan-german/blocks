/*
  ==============================================================================

    DragIndicatorComponent.h
    Created: 29 Oct 2021 2:56:17am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "LabeledSlider.h"

using namespace juce;

class DragIndicatorComponent : public juce::Component {
public:
  DragIndicatorComponent();
  ~DragIndicatorComponent() override;
  void paint(juce::Graphics&) override;
  void resized() override;
  Colour colour;
private:
  float dotSize = 3;
  float spacing = 3;
  int rows = 3;
  int columns = 2;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DragIndicatorComponent)
};
