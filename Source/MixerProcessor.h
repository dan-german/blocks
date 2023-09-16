/*
  ==============================================================================

    MixerProcessor.h
    Created: 15 Jul 2021 7:36:01pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "Processor.h"

class MixerProcessor: public Processor {
public:
  MixerProcessor();
  ~MixerProcessor() override;

  void prepareToPlay(double bufferSize, int sampleRate) override;
  float _getNextValue() override;

  void process(AudioSampleBuffer& inputBuffer) override;
  SmoothedValue<float> pan;
};