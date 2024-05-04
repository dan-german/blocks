/*
  ==============================================================================

    Cursor.h
    Created: 15 Dec 2021 12:13:47pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/ValueAnimator.h"
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;
class Cursor: public juce::Component {
public:
  Cursor() {}
  ~Cursor() override;
  Colour colour;
  void paint(juce::Graphics&) override;
  int size = 9;
  bool is_selecting_ = false;
  void setSelectionMode(bool isSelecting) {
    is_selecting_ = isSelecting;
    repaint();
  }
private:
  ValueAnimator valueAnimator;
  float alpha = 1.0f;
  void setupAnimation();
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Cursor)
};
