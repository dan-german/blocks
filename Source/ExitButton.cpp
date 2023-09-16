/*
  ==============================================================================

    ExitButton.cpp
    Created: 12 Jun 2021 4:50:39am
    Author:  Dan German

  ==============================================================================
*/

#include "ExitButton.h"
#include "ThemeManager.h"

ExitButton::~ExitButton() {}
void ExitButton::resized() {}
void ExitButton::paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {}
ExitButton::ExitButton() : Button("exitButton") { colour = normalColour; }

void ExitButton::paint(juce::Graphics& g) {
  g.setColour(ThemeManager::shared()->getCurrent().two);

  Path first;
  first.startNewSubPath(2, 2);
  first.lineTo(getWidth() - 2, getHeight() - 2);

  Path second;
  second.startNewSubPath(2, getHeight() - 2);
  second.lineTo(getWidth() - 2, 2);

  g.strokePath(first, PathStrokeType(2, PathStrokeType::curved, PathStrokeType::rounded));
  g.strokePath(second, PathStrokeType(2, PathStrokeType::curved, PathStrokeType::rounded));
}