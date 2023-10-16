/*
  ==============================================================================

    PresetButtonComponent.cpp
    Created: 6 Jul 2021 6:15:59am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/controls/PresetButtonContent.h"
#include "settings/Constants.h"
#include "gui/ThemeManager.h"

PresetButtonContent::PresetButtonContent() {
  label.setText("blank", dontSendNotification);
  label.setColour(Label::ColourIds::textColourId, Colour(190, 190, 190));
  label.setJustificationType(Justification::centred);
  label.setFont(Font(14.0f, Font::bold));
  addAndMakeVisible(label);
}

void PresetButtonContent::paint(juce::Graphics& g) {
  g.setColour(ThemeManager::shared()->getCurrent().one);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), getHeight() / 2.0f);
}

void PresetButtonContent::resized() {
  label.setBounds(getLocalBounds());
}