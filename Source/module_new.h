#pragma once
#include "vital/common/synth_parameters.h"
#include "model/id.h"
#include "gui/ModuleColour.h"
#include "model/Index.h"
// #include "vital/synthesis/framework/value.h"

using ValueScale = vital::ValueDetails::ValueScale;
using Type = vital::ValueDetails::Type;

namespace model {
class Module {
public:
  ID id;
  std::string name;
  Index index = { -1, -1 };
  std::vector<std::shared_ptr<vital::ValueDetails>> parameters_;
  // std::vector<vital::Value*> values_;

  enum Category { source, effect, modulator, tab };
  Category category;

  ModuleColour colour = { juce::Colour(237, 237, 237), -1 };
  bool isActive = false;
  bool isChild = false;
  int length = 1;

  inline bool isModulator() { return category == Category::modulator; }
  inline bool isEnvelope() { return id.type == "adsr"; }
  inline bool isOscillator() { return id.type == "osc"; }

  Module(std::string prefix, int number): name(prefix + "_" + std::to_string(number)) { 
    id = { prefix, number };
  }

  void add(vital::ValueDetails parameter) {
    parameter.name = name + "_" + parameter.name;
    auto shared_ptr = std::make_shared<vital::ValueDetails>(parameter);
    parameters_.push_back(shared_ptr);
  }
};
}