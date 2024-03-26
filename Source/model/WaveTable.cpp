/*
  ==============================================================================

    WaveTable.cpp
    Created: 18 Feb 2021 6:07:23pm
    Author:  Dan German

  ==============================================================================
*/

#include "model/WaveTable.h"

#include <algorithm>


WaveTable::WaveTable() noexcept
{
  waveforms.reserve(32);
}

void WaveTable::addWaveform(int length, float* waveform, float topFrequency) {

  Waveform new_waveform { .topFrequency = topFrequency,
                        .data = std::vector<float>(length + 1),
                        .length = length };

  std::copy_n(waveform, length, new_waveform.data.begin());
  new_waveform.data[length] = new_waveform.data[0];

  waveforms.push_back(std::move(new_waveform));
}
