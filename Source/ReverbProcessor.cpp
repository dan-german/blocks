/*
  ==============================================================================

  ReverbProcessor.cpp
  Created: 3 Oct 2020 10:13:28am
  Author:  dange

  ==============================================================================
*/

#include "ReverbProcessor.h"
#include "ReverbModule.h"
#include <juce_dsp/juce_dsp.h>
using Parameters = Model::ReverbModule::Parameters;

ReverbProcessor::ReverbProcessor(): Processor(512) { reverb.setEnabled(true); }
ReverbProcessor::~ReverbProcessor() { }

void ReverbProcessor::prepareToPlay(double sampleRate, int bufferSize) {
  dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32> (bufferSize), 2 };
  reverb.prepare(spec);
}

void ReverbProcessor::process(AudioSampleBuffer& inputBuffer) {
  updateParameters();
  auto block = dsp::AudioBlock<float>(inputBuffer);
  auto context = dsp::ProcessContextReplacing(block);
  reverb.process(context);
}

void ReverbProcessor::updateParameters() {
  float mix = getParameter(Parameters::pMix)->getValue(phase);
  float damping = getParameter(Parameters::pDamping)->getValue(phase);
  float width = getParameter(Parameters::pWidth)->getValue(phase);
  float size = getParameter(Parameters::pSize)->getValue(phase);

  auto parameters = reverb.getParameters();

  parameters.roomSize = size;
  parameters.damping = damping;
  parameters.width = width;
  parameters.wetLevel = mix;
  parameters.dryLevel = 1.0f - mix;

  reverb.setParameters(parameters);
}
