/*
  ==============================================================================

    Cursor.cpp
    Created: 15 Dec 2021 12:13:47pm
    Author:  Dan German

  ==============================================================================
*/

#include "Cursor.h"

Cursor::~Cursor() { }

void Cursor::setupAnimation() {
  valueAnimator.waveType = ValueAnimator::WaveType::triangle;
  valueAnimator.speed = 1.5f;
  valueAnimator.valueAnimatorCallback = [this](float a) {
    alpha = jmap(a, 0.0f, 1.0f, 0.4f, 1.0f);
    repaint();
  };
}

void Cursor::paint(juce::Graphics& g) {
  g.setColour(colour);
  g.fillEllipse(getLocalBounds().toFloat());
}
