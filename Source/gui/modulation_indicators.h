
#pragma once

class ModulationIndicators: public juce::Component {
public:
  Colour colour;
  DrawablePath line_;

  ModulationIndicators() {
    line_.setFill(Colours::red);
    addAndMakeVisible(line_);
  }

  void resized() override { 
    Path p;
    p.addRectangle(getLocalBounds());
    line_.setPath(p);
  }
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationIndicators)
};
