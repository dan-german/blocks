/*
  ==============================================================================

    ConnectionComponent.cpp
    Created: 11 Jun 2021 10:39:21pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ConnectionComponent.h"
#include "settings/Constants.h"

void ConnectionComponent::paint(juce::Graphics& g) { g.fillAll(Colours::transparentBlack); }

ConnectionComponent::ConnectionComponent(): slider(Slider::SliderStyle::LinearHorizontal,
  Slider::TextEntryBoxPosition::TextBoxBelow),
  indicator(1, Colours::white) {
  slider.setColour(Slider::trackColourId, Colour(200, 200, 200));
  slider.setColour(Slider::thumbColourId, Colours::transparentWhite);
  slider.setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);
  slider.setColour(Slider::backgroundColourId, Colour(46, 48, 51));

  slider.setNumDecimalPlacesToDisplay(3);
  slider.getProperties().set("isCenter", true);
  slider.setName("matrixSlider");

  addAndMakeVisible(slider);
  addAndMakeVisible(source);
  addAndMakeVisible(target);

  addAndMakeVisible(indicator);
  indicator.isVertical = false;

  source.setText("lfo 1", dontSendNotification);
  target.setText("filter 1 cutoff", dontSendNotification);

  target.setJustificationType(Justification::centredRight);
  source.setJustificationType(Justification::centredLeft);

  setupBipolarButton();

  addAndMakeVisible(exitButton);
  exitButton.onClick = [this]() { delegate->connectionDeleted(this); };

  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
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
  source.setBounds(horizontalInsets * 2, 0, 100, 20);
  target.setBounds(getWidth() - 100 - horizontalInsets * 2, 0, 100, 20);

  resizeIndicator();
  resizeSlider();
  resizeBipolarButton();
  resizeExitButton();
}

void ConnectionComponent::resizeSlider() {
  int height = 40;
  slider.setBounds(horizontalInsets, indicator.getBottom() - 2, getWidth() - horizontalInsets, height);
}

void ConnectionComponent::resizeIndicator() {
  int horizontalOffset = static_cast<int>(slider.getPositionOfValue(-1.0f));
  indicator.setBounds(horizontalOffset, source.getBottom() + 4, getWidth() - horizontalOffset * 2, 8);
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