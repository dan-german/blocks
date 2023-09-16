/*
  ==============================================================================

    PresetButtonComponent.cpp
    Created: 6 Jul 2021 6:15:59am
    Author:  Dan German

  ==============================================================================
*/

#include "PresetButtonComponent.h"
#include "Constants.h"

PresetButtonComponent::PresetButtonComponent() {
  addAndMakeVisible(content);
  content.setInterceptsMouseClicks(false, false);
}

Component* PresetButtonComponent::getContent() { return &content; }
PresetButtonComponent::~PresetButtonComponent() {}
void PresetButtonComponent::paint(juce::Graphics& g) {}
void PresetButtonComponent::setStrings(StringArray strings) {}
void PresetButtonComponent::setButtonColour(Colour colour) {}
