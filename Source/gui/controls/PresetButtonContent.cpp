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
  Colour background_color = ThemeManager::shared()->getCurrent().background;
  setupLabel(label, "empty", Colour(190, 190, 190), 14.0f);
  setupLabel(left_arrow_, "<", background_color, 18.0f);
  setupLabel(right, ">", background_color, 18.0f);
  ThemeManager::shared()->addListener(this);
}

PresetButtonContent::~PresetButtonContent() {
  ThemeManager::shared()->removeListener(this);
}

void PresetButtonContent::setupLabel(Label& label, String text, Colour color, float fontSize) {
  label.setText(text, dontSendNotification);
  label.setColour(Label::ColourIds::textColourId, color);
  label.setJustificationType(Justification::centred);
  label.setFont(Font(fontSize, Font::bold));
  addAndMakeVisible(label);
}

void PresetButtonContent::paint(juce::Graphics& g) {
  g.setColour(ThemeManager::shared()->getCurrent().one);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), getHeight() / 2.0f);
}

void PresetButtonContent::resized() {
  label.setBounds(getLocalBounds());
  left_arrow_.setBounds(2, 0, 20, getHeight());
  right.setBounds(getWidth() - 22, 0, 20, getHeight());
}

void PresetButtonContent::themeChanged(Theme theme) {
  Colour background_color = ThemeManager::shared()->getCurrent().background;
  left_arrow_.setColour(Label::ColourIds::textColourId, background_color);
  right.setColour(Label::ColourIds::textColourId, background_color);
  repaint();
}