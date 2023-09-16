/*
  ==============================================================================

    DelayProcessor.cpp
    Created: 7 Jun 2021 12:05:48am
    Author:  Dan German

  ==============================================================================
*/

#include "DelayProcessor.h"
#include "DelayModule.h"
#include "NoteHelper.h"

using Parameters = Model::DelayModule::Parameters;

DelayProcessor::DelayProcessor(): Processor(128) { }
DelayProcessor::~DelayProcessor() { }
float DelayProcessor::_getNextValue() { return 0; }
bool DelayProcessor::isActive() { return Processor::isActive(); }

void DelayProcessor::process(AudioSampleBuffer& inputBuffer) {
  float feedback = getParameter(Parameters::pFeedback)->getValue(phase);

  int mode = static_cast<int>(getParameter(Parameters::pSync)->getValue(phase));

  auto timeParameter = getParameter(Parameters::pTime);
  auto value = timeParameter->getValue(phase);

  const float dottedRatio = 2.0f / 3.0f;
  const float tripletRatio = 3.0f / 2.0f;

  auto range = timeParameter->juceParameter->getNormalisableRange();

  if (mode == 1) {
    auto mappedValue = jmap(value, range.start, range.end, 9.0f, 3.0f);
    auto index = int(mappedValue);
    value = NoteHelper::indexToHertz(static_cast<NoteHelper::Duration>(index), bpm);
  } else if (mode == 2) {
    auto mappedValue = jmap(value, range.start, range.end, 9.0f, 3.0f);
    auto index = int(mappedValue);
    value = NoteHelper::indexToHertz(static_cast<NoteHelper::Duration>(index), bpm) * dottedRatio;

  } else if (mode == 3) {
    auto mappedValue = jmap(value, range.start, range.end, 9.0f, 3.0f);
    auto index = int(mappedValue);
    value = NoteHelper::indexToHertz(static_cast<NoteHelper::Duration>(index), bpm) * tripletRatio;
  }

  float delayTime = sampleRate / value;

  if (mode == 0)
    delayTime = value / 1000.0f * sampleRate;

  delayLine.setDelay(delayTime);

  float mix = getParameter(Parameters::pMix)->getValue(phase);
  float dry = 1.0f - mix;

  for (int sample = 0; sample < inputBuffer.getNumSamples(); sample++) {
    for (int channel = 0; channel < inputBuffer.getNumChannels(); channel++) {
      auto inputSample = inputBuffer.getSample(channel, sample);
      auto delayedSample = delayLine.popSample(channel, delayTime);
      auto delayLineInputSample = std::tanh(inputSample + feedback * delayedSample);

      delayLine.pushSample(channel, delayLineInputSample);
      auto outputSample = inputSample * dry + mix * delayedSample;
      inputBuffer.setSample(channel, sample, outputSample);
    }
  }
}

void DelayProcessor::prepareToPlay(double sampleRate, int bufferSize) {
  maxDelay = static_cast<int>(5 * sampleRate);
  Processor::prepareToPlay(sampleRate, bufferSize);
  delayLine = dsp::DelayLine<float>(maxDelay);
  delayLine.prepare({ sampleRate, static_cast<juce::uint32> (bufferSize), 2 });
  delayLine.setDelay(static_cast<float>(sampleRate));
}