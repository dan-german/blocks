/*
  ==============================================================================

    Modulation.cpp
    Created: 11 May 2021 7:43:33am
    Author:  Dan German

  ==============================================================================
*/

#include "connection.h"
#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "vital/synthesis/modules/modulation_connection_processor.h"  

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
  amount_parameter_ = std::make_shared<vital::ValueDetails>();
  amount_parameter_->min = -1.0f;
  amount_parameter_->value = 1.0f;
  bipolar_parameter_ = std::make_shared<vital::ValueDetails>();
}

Connection::Connection(int number): number(number) {
  name = "modulation " + std::to_string(number);
  amount_parameter_ = std::make_shared<vital::ValueDetails>();
  amount_parameter_->min = -1.0f;
  amount_parameter_->value = 1.0f;
  bipolar_parameter_ = std::make_shared<vital::ValueDetails>();
};

Connection::~Connection() { }

bool Connection::operator==(const Connection& rhs) const {
  return this->target == rhs.target && this->source == rhs.source && this->parameter_name_ == rhs.parameter_name_;
}

void Connection::reset() {
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

void Connection::reset(vital::ModulationConnection* vital_connection) {
  amount_parameter_->value_processor = vital_connection->modulation_processor->control_map_["amount"];
  amount_parameter_->set(amount_parameter_->value);
  bipolar_parameter_->value_processor = vital_connection->modulation_processor->control_map_["bipolar"];
  bipolar_parameter_->set(bipolar_parameter_->value);
  vital_connection_ = vital_connection;
}
}