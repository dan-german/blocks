/*
  ==============================================================================

    BoxSliderLooksAndFeel.h
    Created: 6 Dec 2021 9:15:32pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>

class BoxSliderLooksAndFeel: public juce::LookAndFeel_V4 {
public:
  void drawLinearSlider(juce::Graphics& graphics,
    int x,
    int y,
    int width,
    int height,
    float sliderPos,
    float minSliderPos,
    float maxSliderPos,
    const juce::Slider::SliderStyle style,
    juce::Slider& slider) override;
  juce::Slider::SliderLayout getSliderLayout(juce::Slider& slider) override;
};
