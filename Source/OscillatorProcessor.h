/*
  ==============================================================================

    OscillatorProcessor.h
    Created: 19 Feb 2021 12:13:05pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "WaveTableController.h"
#include "Processor.h"
#include "Voice.h"
#include "Module.h"

#define MAX_UNISON 8

class OscillatorProcessor: public Processor {
public:
  OscillatorProcessor();

  float _getNextValue() override;
  void noteStarted(Voice* voice, float frequencyInHertz) override;
  void noteStopped(bool allowTailOff) override;

  void process(AudioSampleBuffer& inputBuffer) override;
  bool hasGainEnvelopes = false;
  ADSR* currentEnvelope;
  ADSR defaultEnvelope;

  ADSR::Parameters* defaultGainADSRParameters;
  ADSR::Parameters* currentGainADSRParameters;

  inline bool isActive() override { return currentEnvelope->isActive(); }
  inline void setADSRParameters(ADSR::Parameters* params) { currentGainADSRParameters = params ? params : defaultGainADSRParameters; }
private:
  SmoothedValue<float> gain = 0.25f;
  int currentWaveTableCode;
  WaveTableController waveTableControllers[MAX_UNISON];
  float frequency = 440.0f;
  void updateWaveform();
  void updateTune();
  void updateGain();
  inline void setFrequency(float f) { this->frequency = f; }
  float semitonesToRatio(float semitones);
  int stacks = 1;
};
