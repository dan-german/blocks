#pragma once
#include "CircleIndicator.h"

class CircleIndicators: public Component {
  OwnedArray<CircleIndicator> indicators;
public:
  void add() {
    addAndMakeVisible(indicators.add(new CircleIndicator()));
  }

  void reset() {
    for (auto indicator : indicators)
      removeChildComponent(indicator);

    indicators.clear();
  }

  void resized() override {
    for (int i = 0; i < indicators.size(); i++) {
      auto indicator = indicators[i];
      auto x = i * 5;
      indicator->setBounds(x, 0, getHeight(), getHeight());
    }
  }

  void addModulationIndicator(Colour colour) {
    auto indicator = indicators.add(new CircleIndicator());
    indicator->setColour(colour);
    addAndMakeVisible(indicator);
    resized();
  }
};