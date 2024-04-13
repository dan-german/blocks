/*
  ==============================================================================

    LabeledSlider.cpp
    Created: 29 Oct 2021 4:06:28am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/controls/LabeledSlider.h"

LabeledSlider::LabeledSlider() {
  addAndMakeVisible(label);
  addAndMakeVisible(box_slider_);
  label.setText("attack", dontSendNotification);
  label.setJustificationType(Justification::centredLeft);
  label.setFont(Font(14));
  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
}

LabeledSlider::~LabeledSlider() {
  ThemeManager::shared()->removeListener(this);
}

void LabeledSlider::resized() {
  label.setBounds(0, 0, getWidth() / 2, getHeight());
  box_slider_.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight());
}

void LabeledSlider::themeChanged(Theme theme) {
  label.setColour(Label::ColourIds::textColourId, theme.two);
}