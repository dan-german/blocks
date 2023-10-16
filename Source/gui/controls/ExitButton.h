/*
  ==============================================================================

    ExitButton.h
    Created: 12 Jun 2021 4:50:39am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class ExitButton: public Button {
public:
  ExitButton();
  ~ExitButton() override;
  void paint(juce::Graphics&) override;
  void resized() override;

  void paintButton(Graphics& g, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
  Colour colour;
  Colour normalColour { 66, 66, 66 };
  Colour highlightedColour { 106, 106, 106 };
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ExitButton)
};
