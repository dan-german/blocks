/*
  ==============================================================================

    Processor.cpp
    Created: 14 Apr 2021 12:32:23am
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/Processor.h"
#include "model/ModuleParameter.h"

Processor::Processor(int bufferSize): buffer(2, bufferSize) { }
Processor::~Processor() { }

void Processor::process() {
  for (int sample = 0; sample < buffer.getNumSamples(); sample++) {
    float value = getNextValue();
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
      buffer.setSample(channel, sample, value);
  }
}

void Processor::prepareToPlay(double sampleRate, int bufferSize) {
  buffer.setSize(2, bufferSize, false, false, false);
  this->sampleRate = float(sampleRate);
}

void Processor::process(AudioSampleBuffer& inputBuffer) {
  for (int sample = 0; sample < inputBuffer.getNumSamples(); sample++) {
    auto value = getNextValue();
    for (int channel = 0; channel < inputBuffer.getNumChannels(); channel++) {
      inputBuffer.addSample(channel, sample, value);
    }
  }
}

float Processor::getNextValue() {
  float value = _getNextValue();
  phase = phase == buffer.getNumSamples() - 1 ? 0 : phase + 1;
  return value;
}

void Processor::setModule(std::shared_ptr<Module> module) {
  this->module = module;
  parameters.clear();

  for (int i = 0; i < module->parameters.size(); i++) {
    auto audioParameter = module->parameters[i]->audioParameter;
    auto parameter = std::make_shared<Parameter>();
    parameter->juceParameter = audioParameter;
    parameters.add(parameter);
  }
}

void Processor::process(int samples) {
  float value;
  for (int i = phase; i < samples; i++) {
    value = getNextValue();
    for (int channel = 0; channel < buffer.getNumChannels(); channel++)
      buffer.setSample(channel, i, value);
  }
}