/*
  ==============================================================================

    LFOModulator.cpp
    Created: 4 Mar 2021 7:24:06am
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/LFOModulator.h"
#include "settings/WaveTableConstants.h"
#include "model/LFOModule.h"

using Parameters = Model::LFOModule::Parameters;

LFOModulator::LFOModulator(): Processor(128) {
  waveTableController.setWaveTable(WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::sawtooth));
  waveTableController.setPhaseIncrement(0.00000867573f);
}

LFOModulator::~LFOModulator() {}
void LFOModulator::noteStopped(bool allowTailOff) {}

float LFOModulator::_getNextValue() {
  int waveform = (int)getParameter(Parameters::pWaveform)->getValue(phase);
  setRate();

  if (waveform == 4) return noiseGenerator.getNextValue();

  setWaveform(waveform);

  return waveTableController.getNextValue();
}

inline void LFOModulator::setRate() {
  int mode = static_cast<int>(getParameter(Parameters::pSynced)->getValue(phase));

  auto rateParameter = getParameter(Parameters::pRate);
  auto rateRange = rateParameter->juceParameter->getNormalisableRange();
  auto value = rateParameter->getValue(phase);

  const float dottedRatio = 2.0f / 3.0f;
  const float tripletRatio = 3.0f / 2.0f;

  if (mode == 1) {
    auto mappedValue = jmap(value, rateRange.start, rateRange.end, 0.0f, 9.0f);
    auto index = int(mappedValue);
    value = NoteHelper::indexToHertz(static_cast<NoteHelper::Duration>(index), bpm);
  } else if (mode == 2) {
    auto mappedValue = jmap(value, rateRange.start, rateRange.end, 0.0f, 9.0f);
    auto index = int(mappedValue);
    value = NoteHelper::indexToHertz(static_cast<NoteHelper::Duration>(index), bpm) * dottedRatio;
  } else if (mode == 3) {
    auto mappedValue = jmap(value, rateRange.start, rateRange.end, 0.0f, 9.0f);
    auto index = int(mappedValue);
    value = NoteHelper::indexToHertz(static_cast<NoteHelper::Duration>(index), bpm) * tripletRatio;
  }

  auto phaseIncrement = value / sampleRate;
  noiseGenerator.frequency = value;
  waveTableController.setPhaseIncrement(phaseIncrement);
}

inline void LFOModulator::setWaveform(int waveform) {
  if (currentWaveformCode == waveform) return;

  currentWaveformCode = waveform;
  WaveTable* waveTable;

  if (waveform == 0)
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::sawtooth);
  else if (waveform == 1)
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::sine);
  else if (waveform == 2)
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::square);
  else if (waveform == 3)
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::triangle);
  else {
    waveTableController.noise = true;
    return;
  }

  waveTableController.setWaveTable(waveTable);
}

void LFOModulator::noteStarted(Voice* voice, float frequencyInHertz) {
  auto mode = getParameter(Parameters::pMode)->getValue(phase);
  if (mode == 1) waveTableController.resetPhase();
}

void LFOModulator::setPhase(int64 samples) {
  auto mode = getParameter(Parameters::pMode)->getValue(phase);
  if (mode == 1) return;

  float frequency = noiseGenerator.frequency; // no special reason to use noiseGenerator.frequency... 

  int samplesPerCycle = static_cast<int>(sampleRate / frequency);
  float offsetInSamples = samples % samplesPerCycle;
  float offset = offsetInSamples / samplesPerCycle;
  waveTableController.setPhase(offset);
}