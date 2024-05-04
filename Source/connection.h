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
struct Connection: Module {
  Connection(Module* target, Module* modulator, int parameterIndex, float magnitude, int number, bool bipolar = false);
  Connection(int number);
  ~Connection();
  vital::ModulationConnection* vital_connection_;

  std::shared_ptr<Module> source;
  std::shared_ptr<Module> target;
  std::string parameter_name_;
  bool hidden = false;

  void reset() override;
  void reset(vital::ModulationConnection* vital_connection_ );
  bool operator==(Connection const& rhs) const;

  std::string getParameterName(std::string name) override;
};
}
