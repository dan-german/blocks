/*
  ==============================================================================

    MixerProcessor.cpp
    Created: 15 Jul 2021 7:36:01pm
    Author:  Dan German

  ==============================================================================
*/

#include "MixerProcessor.h"
#include "MixerModule.h"

using Parameters = Model::MixerModule::Parameters;

MixerProcessor::MixerProcessor(): Processor(128) { }
MixerProcessor::~MixerProcessor() { }
float MixerProcessor::_getNextValue() { return 0; }

void MixerProcessor::prepareToPlay(double sampleRate, int bufferSize) {
  Processor::prepareToPlay(sampleRate, bufferSize);
  pan.reset(0);
}

void MixerProcessor::process(AudioSampleBuffer& inputBuffer) {
  pan.setTargetValue(getParameter(Parameters::pPan)->getValue(phase));
  float gain = 1.0f + getParameter(Parameters::pGain)->getValue(phase) * 2.0f;

  auto panValue = pan.getNextValue();
  float right = (panValue < 0.0f) ? 1.0f + panValue : 1.0f;
  float left = (panValue > 0.0f) ? 1.0f - panValue : 1.0f;

  for (int sample = 0; sample < inputBuffer.getNumSamples(); sample++) {
    auto leftSample = inputBuffer.getSample(0, sample) * left * gain;
    auto rightSample = inputBuffer.getSample(1, sample) * right * gain;

    inputBuffer.setSample(0, sample, leftSample);
    inputBuffer.setSample(1, sample, rightSample);
  }
}