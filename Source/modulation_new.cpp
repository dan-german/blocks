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

Connection::Connection(Module* target,
  Module* modulator,
  int parameterIndex,
  float magnitude,
  int number,
  bool bipolar): source(modulator),
  target(target),
  number(number) {
  name = "modulation " + std::to_string(number);
  // magnitudeParameter = std::shared_ptr<AudioParameterFloat>(new AudioParameterFloat(name + " magnitude", name + " magnitude", NormalisableRange(-1.0f, 1.0f, 0.001f), magnitude));
  // bipolarParameter = std::shared_ptr<AudioParameterBool>(new AudioParameterBool(name + " bipolar", name + " bipolar", bipolar));
}

Connection::Connection(int number): number(number) {
  name = "modulation " + std::to_string(number);
  // magnitudeParameter = std::shared_ptr<AudioParameterFloat>(new AudioParameterFloat(name + " magnitude", name + " magnitude", NormalisableRange(-1.0f, 1.0f, 0.001f), 1.0f));
  // bipolarParameter = std::shared_ptr<AudioParameterBool>(new AudioParameterBool(name + " bipolar", name + " bipolar", false));
};

Connection::~Connection() { }

bool Connection::operator==(const Connection& rhs) const {
  return this->target == rhs.target && this->source == rhs.source && this->parameter_name_ == rhs.parameter_name_;
}

void Connection::reset() {
  // magnitudeParameter->setValue(1.0f);
  // bipolarParameter->setValue(true);
  source = nullptr;
  target = nullptr;
}

void Connection::setMagnitude(float magnitude) {
  // auto normalizedValue = magnitudeParameter->getNormalisableRange().convertTo0to1(magnitude);
  // magnitudeParameter->setValue(normalizedValue);
}

void Connection::setPolarity(bool bipolar) {
  // auto normalizedValue = bipolarParameter->getNormalisableRange().convertTo0to1(bipolar);
  // bipolarParameter->setValue(normalizedValue);
}

bool Connection::isOscGainEnvelope() {
  return true;
  // using Parameters = Model::OscillatorModule::Parameters;
  // return source->isEnvelope() && target->isOscillator() && Parameters(parameterIndex) == Parameters::pGain;
}
}