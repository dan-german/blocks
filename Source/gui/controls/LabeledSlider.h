/*
  ==============================================================================

    LabeledSlider.h
    Created: 29 Oct 2021 4:06:28am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/controls/BoxSlider.h"
#include "gui/ThemeManager.h"

class LabeledSlider: public juce::Component, ThemeListener {
public:
  LabeledSlider();
  ~LabeledSlider() override;

  void paint(juce::Graphics& g) override { 
    // g.fillAll(Colours::red);
  };

  void resized() override;
  Label label;
  BoxSlider box_slider_;
  ID model_id_;
  std::string paramter_name_;
  // ValueDetails
  void themeChanged(Theme theme) override;
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LabeledSlider)
};
