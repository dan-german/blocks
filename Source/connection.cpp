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
  bool bipolar):
  Module("modulation", number),
  source(modulator),
  target(target)
{
  name = "modulation " + std::to_string(number);

  add({ .name = "amount", .min = -1.0, .default_value = 1.0 });
  add({ .name = "bipolar" });
}

Connection::Connection(int number): Module("modulation", number) {
  name = "modulation " + std::to_string(number);
  add({ .name = "amount", .min = -1.0, .default_value = 1.0 });
  add({ .name = "bipolar" });
};

Connection::~Connection() { }

bool Connection::operator==(const Connection& rhs) const {
  return this->target == rhs.target && this->source == rhs.source && this->parameter_name_ == rhs.parameter_name_;
}

void Connection::reset() {
  Module::reset();
  source = nullptr;
  target = nullptr;
}

std::string Connection::getParameterName(std::string name) {
  return name;
  // return name == "amount" ? this->name + " amount" : name;
}

void Connection::reset(vital::ModulationConnection* vital_connection) {
  parameter_map_["amount"]->value_processor = vital_connection->modulation_processor->control_map_.at("amount");
  parameter_map_["amount"]->set(parameter_map_["amount"]->value);

  parameter_map_["bipolar"]->value_processor = vital_connection->modulation_processor->control_map_.at("bipolar");
  parameter_map_["bipolar"]->set(parameter_map_["bipolar"]->value);

  vital_connection_ = vital_connection;
}
}
