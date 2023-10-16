#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class CircleIndicator: public Component, ThemeListener {
public:
  Colour colour;
  CircleIndicator() { }

  void setColour(Colour colour) {
    this->colour = colour;
    repaint();
  }

  void paint(Graphics& g) override {
    g.setColour(colour);
    g.fillEllipse(getLocalBounds().reduced(1).toFloat());
    g.setColour(Colours::black);
    g.drawEllipse(getLocalBounds().reduced(1).toFloat(), 1.5f);
  }

  void resized() override { }

  void themeChanged(Theme theme) override { }
};