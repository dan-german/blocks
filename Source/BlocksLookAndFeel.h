
/*
  ==============================================================================

  BlocksLookAndFeel.h
  Created: 19 Sep 2020 9:36:58pm
  Author:  dange

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class BlocksLookAndFeel: public LookAndFeel_V4 {
public:
  BlocksLookAndFeel();

  void drawLinearSlider(Graphics& graphics,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const Slider::SliderStyle style,
    Slider& slider) override;

  void drawButtonText(Graphics& graphics, TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
  Font getTextButtonFont(TextButton& button, int buttonHeight) override;
  Slider::SliderLayout getSliderLayout(Slider& slider) override;

  void drawButtonBackground(Graphics& graphics, Button& button, const Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;

  void fillTextEditorBackground(Graphics&, int width, int height, TextEditor&) override;
  void drawTextEditorOutline(Graphics&, int width, int height, TextEditor&) override;
  CaretComponent* createCaretComponent(Component* keyFocusOwner) override;
};
