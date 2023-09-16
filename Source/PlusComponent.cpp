/*
  ==============================================================================

    PlusComponent.cpp
    Created: 15 Oct 2020 6:46:08pm
    Author:  Dan German

  ==============================================================================
*/

#include "PlusComponent.h"
#include "ThemeManager.h"
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

PlusComponent::~PlusComponent() {}
void PlusComponent::resized() {}

PlusComponent::PlusComponent(Index index): GridItemComponent({ index, 0 }) {
  setAlpha(0);
  setInterceptsMouseClicks(false, false);
}

void PlusComponent::paint(juce::Graphics& g) {
  g.setColour(ThemeManager::shared()->getCurrent().two);
  g.fillRoundedRectangle(getWidth() / 2 - plusLineLength / 2, getHeight() / 2 - plusLineWidth / 2, plusLineLength, plusLineWidth, 0.0f);
  g.fillRoundedRectangle(getWidth() / 2 - plusLineWidth / 2, getHeight() / 2 - plusLineLength / 2, plusLineWidth, plusLineLength, 0.0f);
}
