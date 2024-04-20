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
  int label_width = getWidth() / 2.25f;
  label.setBounds(0, 0, label_width, getHeight());
  int slider_width = getWidth() - label_width;
  box_slider_.setBounds(label_width, 0, slider_width, getHeight());
}

void LabeledSlider::themeChanged(Theme theme) {
  label.setColour(Label::ColourIds::textColourId, theme.two);
}