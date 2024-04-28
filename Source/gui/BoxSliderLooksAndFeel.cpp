/*
  ==============================================================================

    BoxSliderLooksAndFeel.cpp
    Created: 6 Dec 2021 9:15:32pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/BoxSliderLooksAndFeel.h"
#include "gui/ThemeManager.h"
#include "BoxSliderLooksAndFeel.h"

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
  juce::Slider& slider)
{
  auto theme = ThemeManager::shared()->getCurrent();
  // Colour track_colour = theme.dark ? theme.two : theme.two.brighter(0.40f);
  Colour track_colour = theme.dark ? ThemeManager::shared()->getCurrent().one : theme.two.brighter(0.40f);
  // g.setColour(ThemeManager::shared()->getCurrent().one);
  g.setColour(track_colour);
  const float corner_radius = height / 4.5f;
  auto bounds = slider.getBounds();
  bounds.reduce(1, 1);
  Path clipArea;
  clipArea.addRoundedRectangle(bounds.toFloat(), corner_radius);
  g.reduceClipRegion(clipArea);
  g.fillRoundedRectangle(bounds.toFloat(), corner_radius);

  g.setColour(slider.findColour(Slider::trackColourId));
  if (slider.getProperties()["isCenter"].equals(true)) {
    drawCenter(g, sliderPos, x, width, slider);
  } else {
    drawNormal(g, slider, sliderPos);
  }
}

void BoxSliderLooksAndFeel::drawCenter(juce::Graphics& g, float sliderPos, int x, int width, juce::Slider& slider) {
  bool isOnRight = sliderPos >= (float)x + (float)width * 0.5f;
  if (isOnRight) {
    g.fillRect((float)x + (float)width * 0.5f, 0.0f, sliderPos - width * 0.5f, (float)slider.getHeight());
  } else {
    g.fillRect(sliderPos, 0.0f, (float)x + (float)width * 0.5f - sliderPos, (float)slider.getHeight());
  }
}

void BoxSliderLooksAndFeel::drawNormal(juce::Graphics& g, juce::Slider& slider, float sliderPos) {
  g.fillRect(0.0f, 0.0f, sliderPos, (float)slider.getHeight());
}

Slider::SliderLayout BoxSliderLooksAndFeel::getSliderLayout(Slider& slider) {
  auto layout = Slider::SliderLayout();
  layout.sliderBounds = slider.getBounds();
  return layout;
}