/*
  ==============================================================================

    KeyboardComponent.cpp
    Created: 25 Jul 2021 4:59:08pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/KeyboardComponent.h"
#include "gui/ThemeManager.h"

void KeyboardComponent::paint(juce::Graphics& g) { MidiKeyboardComponent::paint(g); }
void KeyboardComponent::resized() { MidiKeyboardComponent::resized(); }

KeyboardComponent::KeyboardComponent(MidiKeyboardState& state, Orientation orientation): MidiKeyboardComponent(state, orientation), ComponentMovementWatcher(this) {
  themeChanged(ThemeManager::shared()->getCurrent());
  ThemeManager::shared()->addListener(this);
}

void KeyboardComponent::drawBlackNote(int /*midiNoteNumber*/, Graphics& g, Rectangle<float> area, bool isDown, bool isOver, Colour noteFillColour) {
  auto c = noteFillColour;

  if (isDown) c = c.overlaidWith(findColour(keyDownOverlayColourId));
  if (isOver) c = c.overlaidWith(findColour(mouseOverKeyOverlayColourId));

  keyPath.clear();
  keyPath.addRoundedRectangle(area.getX(), area.getY(), area.getWidth(), area.getHeight(), 3.0f, 3.0f, false, false, true, true);

  g.setColour(c);
  g.fillPath(keyPath);
}

void KeyboardComponent::themeChanged(Theme theme) {
  setColour(ColourIds::whiteNoteColourId, theme.one);
  setColour(ColourIds::blackNoteColourId, theme.background);
  setColour(ColourIds::textLabelColourId, Colours::transparentBlack);
  setColour(ColourIds::shadowColourId, Colours::transparentBlack);
  setColour(ColourIds::keyDownOverlayColourId, theme.three);
  setColour(ColourIds::mouseOverKeyOverlayColourId, theme.two);
  setColour(upDownButtonArrowColourId, Colours::transparentBlack);
  setColour(upDownButtonBackgroundColourId, Colours::transparentBlack);
}