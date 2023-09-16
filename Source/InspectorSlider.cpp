/*
  ==============================================================================

    InspectorSlider.cpp
    Created: 20 Sep 2020 6:41:38pm
    Author:  dange

  ==============================================================================
*/

#include "InspectorSlider.h"
#include "Constants.h"

InspectorSlider::~InspectorSlider() { }
void InspectorSlider::setModulatorBipolar(int modulatorIndex, bool bipolar) { indicators[modulatorIndex]->setBipolar(bipolar); }

InspectorSlider::InspectorSlider(Slider::SliderStyle style, Slider::TextEntryBoxPosition textBoxPosition): slider(style, textBoxPosition) {
  titleLabel.setJustificationType(Justification::centred);
  titleLabel.setColour(Label::ColourIds::textColourId, Colour(105, 110, 118));
  addAndMakeVisible(titleLabel);

  addMouseListener(this, true);
  slider.addListener(this);

  slider.setColour(Slider::backgroundColourId, Colour(77, 77, 77));
  slider.setColour(Slider::trackColourId, Colour(200, 200, 200));
  slider.setColour(Slider::thumbColourId, Colours::transparentWhite);
  slider.setColour(Slider::textBoxOutlineColourId, Colours::transparentWhite);

  highlightComponent.setAlpha(0);
  addAndMakeVisible(highlightComponent);
  addAndMakeVisible(slider);
  setValue(1.0f);
}

void InspectorSlider::resized() {
  int labelHeight = 30;
  int sliderY = sliderVerticalInsets;
  int sliderHeight = getHeight() - labelHeight - sliderVerticalInsets;

  slider.setBounds(getLocalBounds().getX(), sliderY, getWidth(), sliderHeight);
  titleLabel.setBounds(getLocalBounds().getX(), sliderHeight - 4, getWidth(), labelHeight);
  highlightComponent.setBounds(getLocalBounds());
  resizeIndicators();
}

void InspectorSlider::setType(InspectorSlider::Type newType) {
  switch (newType) {
  case Type::regular:
    slider.setColour(Slider::trackColourId, Colour(200, 200, 200));
    slider.setColour(Slider::thumbColourId, Colours::transparentWhite);
  case Type::normal:
    break;
  case Type::thumb:
    slider.setColour(Slider::trackColourId, Colours::transparentWhite);
    slider.setColour(Slider::thumbColourId, Colour(200, 200, 200));
    break;
  }
}

void InspectorSlider::sliderValueChanged(Slider* s) {
  auto range = slider.getRange();
  auto zeroToOneValue = jmap(float(slider.getValue()), float(range.getStart()), float(range.getEnd()), 1.0f, 0.0f);

  for (auto indicator : indicators)
    indicator->setMiddleY(zeroToOneValue);
}

void InspectorSlider::setHighlighted(bool highlighted, Colour colour) {
  highlightComponent.valueAnimator.reset();
  if (highlighted) {
    highlightComponent.colour = colour;
    highlightComponent.valueAnimator.start();
  } else {
    highlightComponent.valueAnimator.stop();
    highlightComponent.setAlpha(0);
  }
}

void InspectorSlider::resizeIndicators() {
  auto range = slider.getRange();
  int additionalHeight = 6;
  int bottomY = (int)slider.getPositionOfValue(range.getEnd());
  int topY = (int)slider.getPositionOfValue(range.getStart());
  int sliderHeight = topY - bottomY;
  int sliderWidth = 8;
  int firstIndicatorXPosition = getLocalBounds().getX() + 32;

  for (int i = 0; i < indicators.size(); i++) {
    int sliderCount = (i + 1);
    int x = firstIndicatorXPosition - sliderCount * (sliderWidth);
    auto indicator = indicators[i];
    auto pointSize = sliderWidth - 2;
    indicator->setBounds(x,
      bottomY - additionalHeight / 2 - pointSize / 2,
      sliderWidth,
      sliderHeight + additionalHeight + pointSize);
  }
}

void InspectorSlider::addModulationIndicator(float magnitude, Colour colour, bool bipolar, float value) {
  auto indicator = new ModulationIndicatorComponent(magnitude, colour);
  indicator->setBipolar(bipolar);
  indicator->setCurrentValue(value);
  indicators.add(indicator);

  for (auto i : indicators)
    addAndMakeVisible(i);
}

void InspectorSlider::setModulatorValue(int modulatorIndex, float magnitude, float value) {
  indicators[modulatorIndex]->setMagnitude(magnitude, false);
  indicators[modulatorIndex]->setCurrentValue(value);
  indicators[modulatorIndex]->repaint();
}

void InspectorSlider::removeIndicator(int index) {
  removeChildComponent(indicators[index]);
  indicators.remove(index, true);
}

void InspectorSlider::setValue(float value) {
  slider.setValue(value, dontSendNotification);

  auto range = slider.getRange();
  auto zeroToOneValue = jmap(float(slider.getValue()), float(range.getStart()), float(range.getEnd()), 1.0f, 0.0f);

  for (auto indicator : indicators)
    indicator->setMiddleY(zeroToOneValue);
}
