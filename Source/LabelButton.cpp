#include "LabelButton.h"

LabelButton::~LabelButton() {
  ThemeManager::shared()->removeListener(this);
}

LabelButton::LabelButton() {
  hoverBrightness = 8.0f;
  orbit = false;

  addAndMakeVisible(content);

  text.setJustificationType(Justification::centred);
  text.setFont(Font(fontSize));
  text.setInterceptsMouseClicks(false, false);
  text.setColour(Label::ColourIds::textColourId, colour);

  content.addAndMakeVisible(text);

  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
}

void LabelButton::selectedAnimation(float value, float progress) {
  getContent()->setBounds(getLocalBounds().expanded(value * 2));

  float size = jmap(value, 0.0f, 1.0f, fontSize, selectedFontSize);
  text.setFont(Font(size));

  float brightness = jmap(progress, 0.0f, 1.0f, 0.0f, hoverBrightness);
  text.setColour(Label::ColourIds::textColourId, colour.brighter(brightness));
}

void LabelButton::deselectedAnimation(float value, float progress) {
  float brightness = jmap(progress, 0.0f, 1.0f, hoverBrightness, MAX_ORBIT_BRIGHTNESS);

  float size = jmap(value, 0.0f, 1.0f, selectedFontSize, fontSize);
  text.setFont(Font(size));

  text.setColour(Label::ColourIds::textColourId, colour.brighter(brightness));
}

void LabelButton::deselectedCompletion() {
  getContent()->setBounds(getLocalBounds().reduced(1));
  text.setColour(Label::ColourIds::textColourId, colour);
  text.setFont(Font(fontSize));
}

void LabelButton::selectedCompletion() {
  getContent()->setBounds(getLocalBounds().expanded(1));
  text.setColour(Label::ColourIds::textColourId, colour.brighter(hoverBrightness));
  text.setFont(Font(selectedFontSize));
}

void LabelButton::resized() {
  text.setBounds(getLocalBounds());
  content.setBounds(getLocalBounds().reduced(1));
}

void LabelButton::themeChanged(Theme theme) {
  colour = theme.two.brighter(0.25f);
  text.setColour(Label::ColourIds::textColourId, colour);
}