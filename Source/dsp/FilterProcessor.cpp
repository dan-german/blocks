/*
  ==============================================================================

    FilterProcessor.cpp
    Created: 14 Jul 2021 4:51:40pm
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/FilterProcessor.h"
#include "model/FilterModule.h"

using Parameters = Model::FilterModule::Parameters;

FilterProcessor::FilterProcessor(): Processor(128) { }
FilterProcessor::~FilterProcessor() { }

void FilterProcessor::prepareToPlay(double sampleRate, int bufferSize) {
  Processor::prepareToPlay(sampleRate, bufferSize);

  leftFilter.sampleRate = 44100;
  rightFilter.sampleRate = 44100;
}

void FilterProcessor::process(AudioSampleBuffer& inputBuffer) {
  auto frequency = getParameter(Parameters::pFrequency)->getValue(phase);
  auto q = getParameter(Parameters::pQ)->getValue(phase);
  auto type = getParameter(Parameters::pType)->getValue(phase);

  float mappedQ = jmap(q, LadderFilter::minK, LadderFilter::maxK);

  leftFilter.setFrequency(frequency);
  rightFilter.setFrequency(frequency);

  leftFilter.k = mappedQ;
  rightFilter.k = mappedQ;

  LadderFilter::Type filterType = static_cast<LadderFilter::Type>(int(type)); // 6 types
  leftFilter.type = filterType;
  rightFilter.type = filterType;

  float gain = 1.0f;

  if (filterType == LadderFilter::Type::LPF4 || filterType == LadderFilter::Type::LPF2) {
    gain = jmap(q, 1.0f, 5.0f);
  }

  leftFilter.update();
  rightFilter.update();

  for (int sampleIndex = 0; sampleIndex < inputBuffer.getNumSamples(); sampleIndex++) {
    float inputSampleLeft = inputBuffer.getSample(0, sampleIndex);
    float inputSampleRight = inputBuffer.getSample(1, sampleIndex);

    float outputSampleLeft = leftFilter.process(inputSampleLeft);
    float outputSampleRight = rightFilter.process(inputSampleRight);

    inputBuffer.setSample(0, sampleIndex, outputSampleLeft * gain);
    inputBuffer.setSample(1, sampleIndex, outputSampleRight * gain);
  }
}
