/*
  ==============================================================================

    OscillatorProcessor.cpp
    Created: 19 Feb 2021 12:13:05pm
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/OscillatorProcessor.h"
#include "settings/WaveTableConstants.h"
#include "model/OscillatorModule.h"

using Parameters = Model::OscillatorModule::Parameters;

OscillatorProcessor::OscillatorProcessor(): Processor(128) {
  for (int i = 0; i < MAX_UNISON; i++) {
    waveTableControllers[i] = WaveTableController();
    waveTableControllers[i].setWaveTable(WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::bandlimitedSawtooth));
    waveTableControllers[i].setPhase(float(i) / MAX_UNISON);
  }

  defaultGainADSRParameters = new ADSR::Parameters(0.001f, 0.0f, 1.0f, 0.001f);
  currentGainADSRParameters = defaultGainADSRParameters;

  defaultEnvelope.setParameters(*defaultGainADSRParameters);
  currentEnvelope = &defaultEnvelope;
}

inline float OscillatorProcessor::_getNextValue() {
  float value = 0.0f;
  float volume = std::lerp(0.5f, 0.2f, stacks / float(MAX_UNISON)) * 0.2f; // best way i could think of of lowering volume as unison increases

  for (int i = 0; i < stacks; i++)
    value += waveTableControllers[i].getNextValue() * volume;

  return value * gain.getNextValue();
}

void OscillatorProcessor::process(AudioSampleBuffer& inputBuffer) {
  updateTune();
  updateWaveform();
  updateGain();

  float pan = getParameter(Parameters::pPan)->getValue(phase);
  float right = (pan < 0.0f) ? 1.0f + pan : 1.0f;
  float left = (pan > 0.0f) ? 1.0f - pan : 1.0f;

  for (int sample = 0; sample < inputBuffer.getNumSamples(); sample++) {
    auto value = getNextValue();
    inputBuffer.addSample(0, sample, value * left);
    inputBuffer.addSample(1, sample, value * right);
  }

  currentEnvelope->setParameters(*currentGainADSRParameters);
  currentEnvelope->applyEnvelopeToBuffer(inputBuffer, 0, inputBuffer.getNumSamples());
}

inline void OscillatorProcessor::updateGain() {
  auto value = getParameter(Parameters::pGain)->getValue(phase);
  gain.setTargetValue(value);
}

void OscillatorProcessor::noteStarted(Voice* voice, float frequency) {
  currentEnvelope->noteOn();
  setFrequency(frequency);
}

void OscillatorProcessor::noteStopped(bool allowTailOff) {
  currentEnvelope->noteOff();
}


inline void OscillatorProcessor::updateTune() {
  auto transpose = getParameter(Parameters::pTranspose)->getValue(phase);
  auto fine = getParameter(Parameters::pTune)->getValue(phase);

  float phaseIncrement = (frequency / sampleRate) * semitonesToRatio(transpose + fine);
  stacks = static_cast<int>(getParameter(Parameters::pUnison)->getValue(phase));

  if (stacks > 1) {
    float spread = getParameter(Parameters::pSpread)->getValue(phase) * 6;
    for (int i = 0; i < stacks; i++) {
      float spreadAddition = std::lerp(-spread, spread, i / float(stacks));
      float spreadSemitones = semitonesToRatio(spreadAddition);
      waveTableControllers[i].setPhaseIncrement(phaseIncrement * spreadSemitones);
    }
  } else {
    waveTableControllers[0].setPhaseIncrement(phaseIncrement);
  }
}

inline float OscillatorProcessor::semitonesToRatio(float semitones) {
  float cents = semitones * 100.0f;
  float centsInOctave = 1200.0f;
  return pow(2.0f, cents / centsInOctave);
}

inline void OscillatorProcessor::updateWaveform() {
  int value = (int)getParameter(Parameters::pWave)->getValue(phase);
  if (value == currentWaveTableCode) return;

  currentWaveTableCode = value;
  WaveTable* waveTable;

  for (int i = 0; i < MAX_UNISON; i++) waveTableControllers[i].noise = false;

  if (value == 0) {
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::bandlimitedSawtooth);
  } else if (value == 1) {
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::sine);
  } else if (value == 2) {
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::bandlimitedSquare);
  } else if (value == 3) {
    waveTable = WaveTableConstants::getWaveTable(WaveTableConstants::WaveTableType::bandlimitedTriangle);
  } else {
    for (int i = 0; i < MAX_UNISON; i++) waveTableControllers[i].noise = true;
    return;
  }

  for (int i = 0; i < MAX_UNISON; i++) waveTableControllers[i].setWaveTable(waveTable);
}