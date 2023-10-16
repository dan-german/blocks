/*
  ==============================================================================

    WaveTable.h
    Created: 18 Feb 2021 6:07:23pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "util/Utils.h"

class WaveTable {
public:
  struct Waveform {
    float topFrequency;
    float* data;
    int length;
  };

  WaveTable();
  ~WaveTable();

  void addWaveform(int length, float* waveform, float topFrequency);

  inline Waveform* getWaveform(float phaseIncrement) {
    for (int i = 0; i < size - 1; i++)
      if (phaseIncrement < waveforms[i].topFrequency)
        return &waveforms[i];

    return &waveforms[size - 1];
  }
private:
  Waveform* waveforms = new Waveform[32];
  int size = 0;
};