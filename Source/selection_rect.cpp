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

}

void SelectionRect::paint(juce::Graphics& g) {
  g.fillAll(juce::Colours::red);
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