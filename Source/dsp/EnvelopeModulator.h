/*
  ==============================================================================

    EnvelopeModulator.h
    Created: 4 Mar 2021 7:23:51am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "dsp/Processor.h"

class EnvelopeModulator: public Processor {
private:
  enum Parameter { attack, decay, sustain, release };
  inline void updateParameters();
public:
  void noteStarted(Voice* voice, float frequencyInHertz) override { adsr.noteOn(); }
  void noteStopped(bool allowTailOff) override { adsr.noteOff(); }
public:
  ADSR adsr;
  EnvelopeModulator();
  float _getNextValue() override;
  bool isActive() override { return adsr.isActive(); }
  void prepareToPlay(double sampleRate, int bufferSize) override;
};