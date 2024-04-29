/*
  ==============================================================================

    ConnectionComponent.cpp
    Created: 11 Jun 2021 10:39:21pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ConnectionComponent.h"
#include "settings/Constants.h"
#include "ui_utils.h"

void ConnectionComponent::paint(juce::Graphics& g) {
  // g.fillAll(Colours::transparentBlack); 
  g.setColour(ThemeManager::shared()->getCurrent().one.withAlpha(0.5f));
  float y = getHeight() - 2.0f;
  float insets = horizontalInsets * 2.0f;
  g.fillRect(0.0f, y, (float)getWidth(), 2.0f);
}

ConnectionComponent::ConnectionComponent():
  indicator(1, Colours::white),
  slider(this)
{
  slider.juce_slider_.getProperties().set("isCenter", true);
  slider.value_label_.setVisible(false);
  slider.setName("matrixSlider");
  addAndMakeVisible(slider);

  addAndMakeVisible(indicator);
  indicator.isVertical = false;

  setupLabel(source, Justification::centredLeft);
  setupLabel(target, Justification::centredRight);
  setupLabel(value_label_, Justification::centred);

  value_label_.setText("0.0", dontSendNotification);

  setupBipolarButton();

  addAndMakeVisible(exitButton);
  exitButton.onClick = [this]() { delegate->connectionDeleted(this); };

  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
}

void ConnectionComponent::setupLabel(juce::Label& label, const juce::Justification& justification) {
  label.setFont(Font(14));
  label.setJustificationType(justification);
  addAndMakeVisible(label);
}

ConnectionComponent::~ConnectionComponent() {
  ThemeManager::shared()->removeListener(this);
}

void ConnectionComponent::setupBipolarButton() {
  addAndMakeVisible(bipolarButton);
  bipolarButton.label.setText("bipolar", dontSendNotification);
  bipolarButton.onClick = [this](BlocksButton*, bool bipolar) {
    indicator.setBipolar(bipolar);
    delegate->modulationConnectionBipolarPressed(this, bipolar);
  };
}

void ConnectionComponent::resized() {
  int y = 8;
  source.setBounds(horizontalInsets * 2, y, 100, 20);
  target.setBounds(getWidth() - 100 - horizontalInsets * 2, y, 100, 20);


  resizeIndicator();
  resizeSlider();
  // resizeBipolarButton();
  // resizeExitButton();

  int value_label_width = 80;
  value_label_.setBounds(getWidth() / 2 - value_label_width / 2, slider.getBottom() + 6, value_label_width, 12);
}

void ConnectionComponent::resizeSlider() {
  int height = 12;
  int y = source.getBounds().getBottom() + 10;
  slider.setBounds(horizontalInsets, y, getWidth() - horizontalInsets * 2, height);
}

void ConnectionComponent::resizeIndicator() {
  int horizontalOffset = static_cast<int>(slider.juce_slider_.getPositionOfValue(-1.0f));
  indicator.setBounds(horizontalOffset, slider.getY() - 8, getWidth() - horizontalOffset * 2, 8);
}

void ConnectionComponent::resizeBipolarButton() {
  int width = 52;
  int height = 18;
  bipolarButton.setBounds(10, slider.getBottom() - 18, width, height);
}

void ConnectionComponent::resizeExitButton() {
  int size = 12;
  int y = bipolarButton.getY() + (bipolarButton.getHeight() / 2) - size / 2;
  exitButton.setBounds(getWidth() - size - 12, y, size, size);
}

void ConnectionComponent::handleOscGainEnvelope() {
  bipolarButton.setVisible(false);
  // slider.getProperties().set("disabled", true);
  // slider.setEnabled(false);
}

void ConnectionComponent::reset() {
  slider.getProperties().set("disabled", false);
  slider.setEnabled(true);
  bipolarButton.setVisible(true);
}

void ConnectionComponent::themeChanged(Theme theme) {
  target.setColour(Label::ColourIds::textColourId, theme.two);
  source.setColour(Label::ColourIds::textColourId, theme.two);

  BlocksButton::Config config = {
    .textOnColour = theme.dark ? theme.three : theme.three,
    .textOffColour = theme.dark ? theme.one : theme.three.darker(0.225f),
  };

  bipolarButton.setConfig(config);
}

void ConnectionComponent::sliderAdjusted(BlocksSlider* slider, float value) {
  value_label_.setText(slider->juce_slider_.getTextFromValue(value), dontSendNotification);
}