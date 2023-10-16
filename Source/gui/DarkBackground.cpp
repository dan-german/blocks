#include "DarkBackground.h"

void DarkBackground::paint(juce::Graphics& g) { }
void DarkBackground::resized() { darkener.setPath(path); }

DarkBackground::DarkBackground() {
  colour = Colour(0, 0, 0).withAlpha(0.0f);
  darkener.setFill(FillType(colour));
  addAndMakeVisible(darkener);
  setOpaque(false);
}

void DarkBackground::setVisible(bool visible) {
  if (visible == isVisible()) return;

  if (visible) {
    Component::setVisible(visible);
    auto animation = [this](float value, float) {
      float mappedValue = juce::jmap(value, 0.0f, 1.0f, 0.0f, DarkBackground::Alpha);
      colour = Colour(0, 0, 0).withAlpha(mappedValue);
      darkener.setFill(FillType(colour));
      repaint();
    };
    EasingAnimator::AnimateInput input = { .seconds = 0.08f, .animation = animation };
    animator.animate(input);
  } else {
    auto animation = [this](float value, float) {
      float mappedValue = juce::jmap(value, 0.0f, 1.0f, DarkBackground::Alpha, 0.0f);
      colour = Colour(0, 0, 0).withAlpha(mappedValue);
      darkener.setFill(FillType(colour));
      repaint();
    };
    EasingAnimator::AnimateInput input = {
      .seconds = 0.08f,
      .completion = [this, visible]() { Component::setVisible(visible); },
      .animation = animation
    };
    animator.animate(input);
  }
}

void DarkBackground::mouseUp(const MouseEvent& event) {
  if (!isVisible()) return;
  if (onClick) onClick(this);
}