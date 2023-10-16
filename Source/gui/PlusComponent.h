/*
  ==============================================================================

    PlusComponent.h
    Created: 15 Oct 2020 6:46:08pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "gui/GridItemComponent.h"

// this is a component that draws a +... should be probably replaced with a SVGButton
class PlusComponent: public GridItemComponent {
public:
  PlusComponent(Index index = { -1, -1 });
  ~PlusComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  float plusLineLength = 10;
  float plusLineWidth = 2;
  Colour colour = Colour(66, 66, 66);
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlusComponent)
};
