/*
  ==============================================================================

    PresetButtonComponent.cpp
    Created: 6 Jul 2021 6:15:59am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/controls/PresetButtonComponent.h"
#include "settings/Constants.h"

PresetButtonComponent::PresetButtonComponent() {
  addAndMakeVisible(content);
  content.setInterceptsMouseClicks(false, false);
}

Component* PresetButtonComponent::getContent() { return &content; }
PresetButtonComponent::~PresetButtonComponent() {}
void PresetButtonComponent::paint(juce::Graphics& g) {}
void PresetButtonComponent::setStrings(std::vector<std::string> strings) {}
void PresetButtonComponent::setButtonColour(Colour colour) {}

void PresetButtonComponent::mouseUp(const juce::MouseEvent& event) {
  int x = event.getPosition().getX();
  bool pressed_left_area = x < getWidth() / 7;
  bool pressed_right_area = x > getWidth() - getWidth() / 7;

  if (pressed_left_area || pressed_right_area) {
    if (on_arrow_click_) on_arrow_click_(pressed_right_area);
  } else {
    if (on_click_) on_click_();
  }
}