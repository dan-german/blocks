/*
  ==============================================================================

    DelayProcessor.h
    Created: 7 Jun 2021 12:05:48am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "dsp/Processor.h"
#include <juce_dsp/juce_dsp.h>

class DelayProcessor: public Processor {
private:
  juce::dsp::DelayLine<float> delayLine;
  int maxDelay;
public:
  DelayProcessor();
  ~DelayProcessor() override;

  void process(AudioSampleBuffer& inputBuffer) override;
  float _getNextValue() override;
  void prepareToPlay(double sampleRate, int bufferSize) override;
  bool isActive() override;
};
