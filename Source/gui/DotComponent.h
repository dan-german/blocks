/*
  ==============================================================================

    DotComponent.h
    Created: 17 Oct 2020 2:25:47am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "DotComponent.h"
#include "gui/GridItemComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class DotComponent: public Component {
public:
  DotComponent();
  ~DotComponent() override;

  void paint(juce::Graphics&) override;
  void mouseMove(const MouseEvent& event) override;
  void setColour(Colour colour);
private:
  Colour colour;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(DotComponent)
};
