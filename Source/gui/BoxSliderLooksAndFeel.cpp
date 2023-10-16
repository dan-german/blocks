/*
  ==============================================================================

    BoxSliderLooksAndFeel.cpp
    Created: 6 Dec 2021 9:15:32pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/BoxSliderLooksAndFeel.h"
#include "gui/ThemeManager.h"

using namespace juce;
void BoxSliderLooksAndFeel::drawLinearSlider(juce::Graphics& g,
  int x,
  int y,
  int width,
  int height,
  float sliderPos,
  float minSliderPos,
  float maxSliderPos,
  const juce::Slider::SliderStyle style,
  juce::Slider& slider) {
  g.setColour(ThemeManager::shared()->getCurrent().one);
  auto cornerRadius = 4.5f;

  Path clipArea;
  clipArea.addRoundedRectangle(slider.getBounds().toFloat(), cornerRadius);
  g.reduceClipRegion(clipArea);
  g.fillRoundedRectangle(slider.getBounds().toFloat(), cornerRadius);

  g.setColour(slider.findColour(Slider::trackColourId));
  g.fillRect(0.0f, 0.0f, sliderPos, (float)slider.getHeight());
}

Slider::SliderLayout BoxSliderLooksAndFeel::getSliderLayout(Slider& slider) {
  auto layout = Slider::SliderLayout();
  layout.sliderBounds = slider.getBounds();
  return layout;
}