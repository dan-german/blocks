/*
  ==============================================================================

    PresetButtonComponent.h
    Created: 6 Jul 2021 6:15:59am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;
#include "BinaryData.h"
#include "gui/controls/PresetButtonContent.h"

class PresetButtonComponent: public BaseButton {
public:
  PresetButtonContent content;
  struct Listener;
  Listener* listener = nullptr;
  std::function<void(bool)> on_arrow_click_;

  PresetButtonComponent();
  ~PresetButtonComponent() override;

  Label label;
  void paint(juce::Graphics&) override;
  void setStrings(StringArray strings);
  void setButtonColour(Colour colour) override;
  void mouseUp(const juce::MouseEvent& event) override;
  Component* getContent() override;
private:
  int index = 0;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetButtonComponent)
};