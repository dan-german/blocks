#include "selection_rect.h"
#include "settings/Constants.h"
#include "gui/GridComponent.h"
#include "settings/GridConfigs.h"
#include "gui/ThemeManager.h" 

SelectionRect::SelectionRect() {
  ThemeManager::shared()->addListener(this);
}

SelectionRect::~SelectionRect() {
  ThemeManager::shared()->removeListener(this);
}

void SelectionRect::resized() {
  stroke_.clear();
  stroke_.addRectangle(getLocalBounds().toFloat());
}

void SelectionRect::paint(juce::Graphics& g) {
  Colour c(175, 188, 255);
  // g.setColour(c.withAlpha(0.5f));
  g.fillAll(c.withAlpha(0.1f));
  g.setColour(c);
  g.strokePath(stroke_, PathStrokeType(2.0f));
}

void SelectionRect::themeChanged(Theme theme) {
  // for (auto& slider : level_sliders_) {
  //   slider->slider.setColour(Slider::ColourIds::trackColourId, theme.two.withAlpha(0.45f));
  // }
  // for (auto& slider : pan_sliders_) {
  //   slider->slider.setColour(Slider::ColourIds::trackColourId, theme.two.withAlpha(0.45f));
  // }
}

void SelectionRect::reset() {
}