/*
  ==============================================================================

    WaveTable.cpp
    Created: 18 Feb 2021 6:07:23pm
    Author:  Dan German

  ==============================================================================
*/

#include "model/WaveTable.h"

WaveTable::WaveTable() { }
WaveTable::~WaveTable() { }

void WaveTable::addWaveform(int length, float* waveform, float topFrequency) {
  waveforms[size].data = new float[length + 1];
  waveforms[size].length = length;
  waveforms[size].topFrequency = topFrequency;

  for (long i = 0; i < length; i++)
    waveforms[size].data[i] = waveform[i];

  waveforms[size].data[length] = waveforms[size].data[0];
  size++;
}
