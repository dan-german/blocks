/*
  ==============================================================================

    WaveTable.h
    Created: 18 Feb 2021 6:07:23pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "util/Utils.h"

#include <vector>

class WaveTable {
public:
  struct Waveform {
    float topFrequency;
    std::vector<float> data;
    int length; // data has +1 sample then length
  };

  WaveTable() noexcept;
  WaveTable(const WaveTable&) = delete;
  WaveTable(WaveTable&&) noexcept = default;
  WaveTable& operator=(const WaveTable&) = delete;
  WaveTable& operator=(WaveTable&&) noexcept = default;

  void addWaveform(int length, float* waveform, float topFrequency);

  inline Waveform* getWaveform(float phaseIncrement) {
    for (auto& waveform : waveforms) {
      if (phaseIncrement < waveform.topFrequency)
        return &waveform;
    }
    return &waveforms.back();

  }
private:
  std::vector<Waveform> waveforms;
};
