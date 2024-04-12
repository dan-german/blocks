/*
  ==============================================================================

    HighlightComponent.cpp
    Created: 8 Mar 2021 2:37:51pm
    Author:  Dan German

  ==============================================================================
*/

#include "HighlightComponent.h"
// #include "dsp/BasicWaveforms.h"
using namespace juce;

HighlightComponent::~HighlightComponent() { }
void HighlightComponent::resized() { }

HighlightComponent::HighlightComponent() {
  valueAnimator.speed = 2.0f;
  valueAnimator.waveType = ValueAnimator::WaveType::triangle;
  valueAnimator.valueAnimatorCallback = [this](float value) {
    setAlpha(value * 0.75f + 0.25f);
  };
}

void HighlightComponent::paint(juce::Graphics& g) {
  Path path;
  auto r = getBounds().reduced(6);
  path.addRoundedRectangle(r, 22.0f);
  g.setColour(colour);
  g.fillPath(path);
}
