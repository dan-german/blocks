/*
  ==============================================================================

    EnvelopePath.h
    Created: 3 Sep 2021 8:44:55pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class EnvelopePath: public juce::Component {
public:
  EnvelopePath();
  ~EnvelopePath() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  Colour colour;

  void setAttack(float a);
  void setDecay(float d);
  void setSustain(float s);
  void setRelease(float r);
private:
  float attackStartX = 0;
  float attackEndX = 0;
  float attackMaxX = 0;
  float attackMaxY = 0;
  float attackMinY = 0;

  float decayMaxX = 0;
  float decayEndX = 0;

  float sustainMaxX = 0;
  float sustainY = 0;

  float releaseEndX = 0;

  float attack = 1.0f;
  float decay = 1.0f;
  float sustain = 1.0f;
  float release = 1.0f;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EnvelopePath)
};
