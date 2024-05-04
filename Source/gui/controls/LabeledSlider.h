/*
  ==============================================================================

    LabeledSlider.h
    Created: 29 Oct 2021 4:06:28am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/controls/blocks_slider.h"
#include "gui/ThemeManager.h"

class LabeledSlider: public juce::Component, ThemeListener {
public:
  Label label;
  BlocksSlider box_slider_;

  LabeledSlider(BlocksSlider::Listener* blocks_slider_listener);
  ~LabeledSlider() override;
  void resized() override;
  void themeChanged(Theme theme) override;
  void paint(juce::Graphics& g) override { 
  };
};
