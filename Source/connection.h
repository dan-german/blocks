/*
  ==============================================================================

    Modulation.h
    Created: 11 May 2021 7:43:33am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include <memory>
#include "module_new.h"
#include <juce_audio_processors/juce_audio_processors.h>
#include "vital/common/synth_parameters.h"
#include "vital/common/synth_types.h"

namespace model {
struct Connection {
  Connection(Module* target, Module* modulator, int parameterIndex, float magnitude, int number, bool bipolar = false);
  Connection(int number);
  ~Connection();
  std::shared_ptr<vital::ValueDetails> magnitude_parameter_;
  std::shared_ptr<vital::ValueDetails> bipolar_parameter_;
  vital::ModulationConnection* vital_connection_;

  int id = 0;
  int number;
  std::string name;
  std::shared_ptr<Module> source;
  std::shared_ptr<Module> target;
  std::string parameter_name_;

  bool isOscGainEnvelope();

  void reset();
  void setMagnitude(float magnitude);
  void setPolarity(bool bipolar);
  bool operator==(Connection const& rhs) const;
};
}
