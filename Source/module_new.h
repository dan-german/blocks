#pragma once
#include "vital/common/synth_parameters.h"
#include "model/id.h"
#include "vital/synthesis/producers/synth_oscillator.h"
#include "gui/ModuleColour.h"
#include "model/Index.h"

using ValueScale = vital::ValueDetails::ValueScale;
using Type = vital::ValueDetails::Type;
using Osc = vital::SynthOscillator;

namespace model {
class Module {
public:
  ID id;
  std::string name;
  Index index = { -1, -1 };
  std::vector<vital::ValueDetails> parameters_;

  enum Category { source, effect, modulator, tab };
  Category category;

  ModuleColour colour = { juce::Colour(237, 237, 237), -1 };
  bool isActive = false;
  bool isChild = false;
  int length = 1;

  inline bool isModulator() { return category == Category::modulator; }
  inline bool isEnvelope() { return id.type == "adsr"; }
  inline bool isOscillator() { return id.type == "osc"; }

  Module(std::string prefix): name(prefix) { }

  void add(vital::ValueDetails parameter) {
    parameter.name = name + "_" + parameter.name;
    parameters_.push_back(parameter);
  }
};
}