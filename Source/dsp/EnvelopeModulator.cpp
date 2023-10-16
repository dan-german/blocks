/*
  ==============================================================================

    EnvelopeModulator.cpp
    Created: 4 Mar 2021 7:23:51am
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/EnvelopeModulator.h"
#include "model/EnvelopeModule.h"

using Parameters = Model::EnvelopeModule::Parameters;
EnvelopeModulator::EnvelopeModulator(): Processor(128) {}

float EnvelopeModulator::_getNextValue() {
  updateParameters();
  return jmap(adsr.getNextSample(), 0.0f, 1.0f, -1.0f, 1.0f);;
}

void EnvelopeModulator::prepareToPlay(double sampleRate, int bufferSize) {
  Processor::prepareToPlay(sampleRate, bufferSize);
  adsr.setSampleRate(sampleRate);
}

void EnvelopeModulator::updateParameters() {
  auto parameters = adsr.getParameters();
  parameters.attack = getParameter(Parameters::attack)->getValue(phase);
  parameters.decay = getParameter(Parameters::decay)->getValue(phase);
  parameters.sustain = getParameter(Parameters::sustain)->getValue(phase);
  parameters.release = getParameter(Parameters::release)->getValue(phase);
  adsr.setParameters(parameters);
}