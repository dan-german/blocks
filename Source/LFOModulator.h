/*
  ==============================================================================

    LFOModulator.h
    Created: 4 Mar 2021 7:24:06am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "WaveTable.h"
#include "Processor.h"
#include "WaveTableController.h"
#include "NoiseGenerator.h"

class LFOModulator: public Processor {
public:
  enum class LFOType { saw, sine, triangle, square };
  LFOModulator();
  virtual ~LFOModulator() override;
  float _getNextValue() override;
  void noteStarted(Voice* voice, float frequencyInHertz) override;
  void noteStopped(bool allowTailOff) override;
  void setPhase(int64 samples) override;
private:
  WaveTableController waveTableController;
  NoiseGenerator noiseGenerator;
  int bufferSize = 512;
  int currentWaveformCode = 0;
  float frequency = 0;
  void setWaveform(int waveform);
  void setRate();
};
