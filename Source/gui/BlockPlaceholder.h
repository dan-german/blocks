/*
  ==============================================================================

    BlockPlaceholder.h
    Created: 21 Dec 2021 4:56:52pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/GraphicsTimer.h"
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class BlockPlaceholder: public juce::Component, GraphicsTimer {
public:
  BlockPlaceholder();
  ~BlockPlaceholder() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  float offset = 0.0f;
  float lineLength = 0.0f;
  void setColour(bool red) { colour = red ? Colour(193, 47, 47) : Colour(122, 223, 67); }
  void setLength(int length);
protected:
  void update(const float secondsSinceLastUpdate) override;
private:
  float dotCount = 0.0f;
  Colour colour;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockPlaceholder)
};
