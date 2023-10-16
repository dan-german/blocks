/*
  ==============================================================================

    BlocksButton.h
    Created: 26 Jun 2021 5:16:57pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/GraphicsTimer.h"
#include "gui/EasingAnimator.h"
#include "gui/base/BaseButton.h"
#include "juce_gui_basics/juce_gui_basics.h"
#include "settings/Constants.h"

using namespace juce;

class SVGButton: public BaseButton {
public:
  SVGButton();
  void setButtonColour(Colour colour) override;
  void setSVG(const void* svgData, const size_t svgDataNumBytes);
protected:
  Component* getContent() override;
  void paint(juce::Graphics&) override;
private:
  Colour _currentColor = Colours::white; // shouldn't be touched other than in setIconColour
  std::unique_ptr<DrawableButton> drawableButton;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SVGButton)
};