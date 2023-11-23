/*
  ==============================================================================

    Modulation.cpp
    Created: 11 May 2021 7:43:33am
    Author:  Dan German

  ==============================================================================
*/

#include "modulation_new.h"
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
// #include "OscillatorModule.h"

// using namespace Model;
// using namespace juce;
namespace model {

Modulation::Modulation(Module* target,
  Module* modulator,
  int parameterIndex,
  float magnitude,
  int number,
  bool bipolar): source(modulator),
  target(target),
  parameterIndex(parameterIndex),
  number(number) {
  name = "modulation " + std::to_string(number);
  // magnitudeParameter = std::shared_ptr<AudioParameterFloat>(new AudioParameterFloat(name + " magnitude", name + " magnitude", NormalisableRange(-1.0f, 1.0f, 0.001f), magnitude));
  // bipolarParameter = std::shared_ptr<AudioParameterBool>(new AudioParameterBool(name + " bipolar", name + " bipolar", bipolar));
}

Modulation::Modulation(int number): number(number) {
  name = "modulation " + std::to_string(number);
  // magnitudeParameter = std::shared_ptr<AudioParameterFloat>(new AudioParameterFloat(name + " magnitude", name + " magnitude", NormalisableRange(-1.0f, 1.0f, 0.001f), 1.0f));
  // bipolarParameter = std::shared_ptr<AudioParameterBool>(new AudioParameterBool(name + " bipolar", name + " bipolar", false));
};

Modulation::~Modulation() { }

bool Modulation::operator==(const Modulation& rhs) const {
  return this->target == rhs.target && this->source == rhs.source && this->parameterIndex == rhs.parameterIndex;
}

void Modulation::reset() {
  // magnitudeParameter->setValue(1.0f);
  // bipolarParameter->setValue(true);
  source = nullptr;
  target = nullptr;
}

void Modulation::setMagnitude(float magnitude) {
  // auto normalizedValue = magnitudeParameter->getNormalisableRange().convertTo0to1(magnitude);
  // magnitudeParameter->setValue(normalizedValue);
}

void Modulation::setPolarity(bool bipolar) {
  // auto normalizedValue = bipolarParameter->getNormalisableRange().convertTo0to1(bipolar);
  // bipolarParameter->setValue(normalizedValue);
}

bool Modulation::isOscGainEnvelope() {
  return true;
  // using Parameters = Model::OscillatorModule::Parameters;
  // return source->isEnvelope() && target->isOscillator() && Parameters(parameterIndex) == Parameters::pGain;
}
}