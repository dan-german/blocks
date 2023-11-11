#pragma once
#include "vital/common/synth_parameters.h"
#include "model/id.h"
#include "vital/synthesis/producers/synth_oscillator.h"

using ValueScale = vital::ValueDetails::ValueScale;
using Type = vital::ValueDetails::Type;
using Osc = vital::SynthOscillator;

namespace model {
class Module {
public:
  ID id;
  std::string prefix_;
  std::vector<vital::ValueDetails> parameters_;

  Module(std::string prefix): prefix_(prefix) { }

  void add(vital::ValueDetails parameter) {
    parameter.name = prefix_ + "_" + parameter.name;
    parameters_.push_back(parameter);
  }
};
}