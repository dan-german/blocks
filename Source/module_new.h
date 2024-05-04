#pragma once
#include "vital/common/synth_parameters.h"
#include "model/id.h"
#include "gui/ModuleColour.h"
#include "model/Index.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/value_bridge.h"

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

  Module(std::string prefix, int number): name(prefix + " " + std::to_string(number)) {
    id = { prefix, number };
    display_name = prefix + " " + std::to_string(number);
  }

  virtual std::string getParameterName(int index) { return parameters_[index]->name; }
  virtual std::string getParameterName(std::string name) { return name; }
  virtual std::shared_ptr<vital::ValueDetails> getParameter(std::string name) { return parameter_map_[getParameterName(name)]; }

  void add(vital::ValueDetails parameter) {
    parameter.display_name = parameter.display_name != "" ? parameter.display_name : parameter.name;
    auto shared_ptr = std::make_shared<vital::ValueDetails>(parameter);
    parameters_.push_back(shared_ptr);
    parameter_map_[parameter.name] = shared_ptr;
    parameter_map_[parameter.name]->value  = parameter.default_value;
  }

  virtual void reset() {
    for (auto parameter : parameters_) {
      parameter->set(parameter->default_value);
    }
  }
};

class Block: public Module {
public:
  Index index = { -1, -1 };
  Block(std::string prefix, int number): Module(prefix, number) {
    // category = Category::source;
  };
  void reset() override {
    Module::reset();
    index = { -1, -1 };
  };
};
} // namespace model