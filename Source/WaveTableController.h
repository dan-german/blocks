/*
  ==============================================================================

    WaveTableController.h
    Created: 21 Apr 2021 3:48:41pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "WaveTable.h"
#include <algorithm>

class WaveTableController {
private:
  WaveTable* waveTable;
  WaveTable::Waveform* currentWaveform;

  float phaseAccumulator = 0.0f;
  float phaseIncrement = 0.0001f;

  inline void incrementPhase() {
    phaseAccumulator += phaseIncrement;

    if (phaseAccumulator >= 1.0f)
      phaseAccumulator -= 1.0f;
  }
public:
  bool noise = false;

  inline void setWaveTable(WaveTable* waveform) {
    this->waveTable = waveform;
    currentWaveform = waveTable->getWaveform(this->phaseIncrement);
  }

  inline void setPhaseIncrement(float newPhaseIncrement) {
    auto clamped = std::clamp(newPhaseIncrement, 0.000000022675737f, 0.9f);
    if (clamped == this->phaseIncrement)
      return;

    this->phaseIncrement = clamped;
    currentWaveform = waveTable->getWaveform(clamped);
  }

  inline float getNextValue() {
    if (noise) return 1 - 2.0f * rand() / RAND_MAX;

    float fractionalIndex = phaseAccumulator * currentWaveform->length;
    incrementPhase();
    return Utils::decimalSubscript(currentWaveform->data, fractionalIndex);
  }

  inline void resetPhase() { phaseAccumulator = 0; }
  inline float getPhase() { return phaseAccumulator; }
  inline void setPhase(float phase) { phaseAccumulator = phase; }
  inline float getPhaseIncrement() { return this->phaseIncrement; }
  inline int getWaveTableLength() { return currentWaveform->length; }
};
