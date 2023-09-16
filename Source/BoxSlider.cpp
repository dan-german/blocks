#include "BoxSlider.h"
#include "ThemeManager.h"

void BoxSlider::sliderValueChanged(Slider* slider) {
  valueLabel.setText(slider->getTextFromValue(slider->getValue()), dontSendNotification);
}

BoxSlider::~BoxSlider() {
  slider.setLookAndFeel(nullptr);
  ThemeManager::shared()->removeListener(this);
}

BoxSlider::BoxSlider() {
  slider.setLookAndFeel(&lnf);
  slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);

  addAndMakeVisible(slider);
  slider.addListener(this);
  slider.setName("modulatorSlider");
  setupLabel();
  ThemeManager::shared()->addListener(this);
}

void BoxSlider::resized() {
  slider.setBounds(getLocalBounds());
  valueLabel.setBounds(getLocalBounds());
}

void BoxSlider::setupLabel() {
  addAndMakeVisible(valueLabel);
  valueLabel.setText("", dontSendNotification);
  valueLabel.setJustificationType(Justification::centred);
  valueLabel.setInterceptsMouseClicks(false, false);
  valueLabel.setColour(Label::ColourIds::textColourId, Colour(200, 200, 200));
  valueLabel.setFont(Font(13));
}

void BoxSlider::themeChanged(Theme theme) {
  valueLabel.setColour(Label::ColourIds::textColourId, theme.two.brighter(0.3f));
}