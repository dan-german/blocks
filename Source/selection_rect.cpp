#include "selection_rect.h"
#include "settings/Constants.h"
#include "gui/GridComponent.h"
#include "gui/ThemeManager.h" 

SelectionRect::SelectionRect() {
  ThemeManager::shared()->addListener(this);
}

SelectionRect::~SelectionRect() {
  ThemeManager::shared()->removeListener(this);
}

void SelectionRect::resized() {
  stroke_.clear();
  stroke_.addRectangle(getLocalBounds());
}

void SelectionRect::paint(juce::Graphics& g) {
  Colour c(175, 188, 255);
  g.fillAll(c.withAlpha(0.1f));
  g.setColour(c);
  g.strokePath(stroke_, PathStrokeType(2.0f));
}

void SelectionRect::themeChanged(Theme theme) {
}

void SelectionRect::reset() {
}