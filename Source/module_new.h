#pragma once
#include "vital/common/synth_parameters.h"
#include "model/id.h"
#include "gui/ModuleColour.h"
#include "model/Index.h"

using ValueScale = vital::ValueDetails::ValueScale;

namespace model {
class Module {
public:
  ID id;
  std::string name;
  std::string display_name;
  std::vector<std::shared_ptr<vital::ValueDetails>> parameters_;
  std::map<std::string, std::shared_ptr<vital::ValueDetails>> parameter_map_;

  enum Category { source, effect, modulator, tab };
  Category category;

  ModuleColour colour = { juce::Colour(237, 237, 237), -1 };
  bool isActive = false;
  bool isChild = false;
  int length = 1;

  inline bool isModulator() { return category == Category::modulator; }
  inline bool isEnvelope() { return id.type == "envelope"; }
  inline bool isOscillator() { return id.type == "osc"; }

  Module(std::string prefix, int number): name(prefix + "_" + std::to_string(number)) {
    id = { prefix, number };
    display_name = prefix + " " + std::to_string(number);
  }

  void add(vital::ValueDetails parameter) {
    parameter.display_name = parameter.display_name != "" ? parameter.display_name : parameter.name;
    std::string short_name = parameter.name;
    parameter.name = short_name;//name + "_" + parameter.name;
    auto shared_ptr = std::make_shared<vital::ValueDetails>(parameter);
    parameters_.push_back(shared_ptr);
    parameter_map_[short_name] = shared_ptr;
  }
};

class Block: public Module {
public:
  Index index = { -1, -1 };
  Block(std::string prefix, int number): Module(prefix, number) {
    // category = Category::source;
  };
  // void reset() {
  //   Module::reset();
  //   index = { -1, -1 };
  // };
};
} // namespace model