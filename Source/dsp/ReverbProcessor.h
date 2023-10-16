/*
  ==============================================================================

  ReverbProcessor.h
  Created: 3 Oct 2020 10:13:28am
  Author:  dange

  ==============================================================================
*/

#pragma once
#include "dsp/Processor.h"
#include <juce_dsp/juce_dsp.h>
class ReverbProcessor: public Processor {
public:
  ReverbProcessor();
  ~ReverbProcessor() override;
  void prepareToPlay(double sampleRate, int bufferSize) override;
  void process(AudioSampleBuffer& inputBuffer) override;
private:
  void updateParameters();
  juce::dsp::Reverb reverb;
};
