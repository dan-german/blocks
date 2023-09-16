/*
  ==============================================================================

    FilterProcessor.h
    Created: 14 Jul 2021 4:51:40pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "Processor.h"
#include "LadderFilter.h"

class FilterProcessor: public Processor {
public:
  FilterProcessor();
  ~FilterProcessor() override;

  void prepareToPlay(double bufferSize, int sampleRate) override;
  void process(AudioSampleBuffer& inputBuffer) override;
private:
  enum Parameter { pType, pCutoff, pResonance };
  LadderFilter leftFilter;
  LadderFilter rightFilter;
};
