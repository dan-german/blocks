/*
  ==============================================================================

    KeyboardComponent.h
    Created: 25 Jul 2021 4:59:08pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <juce_audio_utils/juce_audio_utils.h>
#include "gui/ThemeListener.h"
#include "gui/ThemeManager.h"

using namespace juce;

class KeyboardComponent: public MidiKeyboardComponent, public ThemeListener, ComponentMovementWatcher {
private:
  Path keyPath;
  void themeChanged(Theme theme) override;
  virtual void componentMovedOrResized(bool wasMoved, bool wasResized) override {};
  virtual void componentPeerChanged() override {};
  virtual void componentVisibilityChanged() override { if (isShowing()) grabKeyboardFocus(); };
public:
  KeyboardComponent(MidiKeyboardState& state, Orientation orientation);
  ~KeyboardComponent() override { ThemeManager::shared()->removeListener(this); };

  void paint(juce::Graphics&) override;
  void resized() override;
  void drawBlackNote(int /*midiNoteNumber*/, Graphics& g, Rectangle<float> area, bool isDown, bool isOver, Colour noteFillColour) override;
};
