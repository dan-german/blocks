/*
  ==============================================================================

    BlocksButton.cpp
    Created: 26 Jun 2021 5:16:57pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/controls/BlocksButton.h"

BlocksButton::BlocksButton(Colour buttonColour, Type type): type(type) {
  label.setText("trigger", dontSendNotification);
  label.setJustificationType(Justification::centred);
  label.setFont(Font(13));
  addAndMakeVisible(label);
  addMouseListener(this, true);
}

BlocksButton::~BlocksButton() {}

void BlocksButton::resized() {
  outlinePath.addRoundedRectangle(getLocalBounds(), getHeight() / 2.0f);
  clipAreaPath.addRoundedRectangle(getLocalBounds(), getHeight() / 2.0f);
  label.setBounds(getLocalBounds());
}

void BlocksButton::mouseUp(const MouseEvent& event) {
  setState(!selected);
  if (onClick) onClick(this, selected);
}

void BlocksButton::setConfig(Config config) {
  this->config = config;
  label.setColour(Label::ColourIds::textColourId, selected ? config.textOnColour : config.textOffColour);
}

void BlocksButton::setState(bool chosen) {
  this->selected = chosen;
  label.setColour(Label::ColourIds::textColourId, chosen ? config.textOnColour : config.textOffColour);
  repaint();
}