#pragma once
#include "module_new.h"

namespace model {
class ChorusModule: public Block {
public:
  ChorusModule(int number): Block("chorus", number) {
    add({ .name = "voices", .min = 1.0, .max = 4.0, .display_multiply = 4.0, .value_scale = ValueScale::kIndexed, .modulatable = false });
    add({ .name = "depth", .default_value = 0.5f });
    add({ .name = "feedback", .min = -0.95f, .max = 0.95f, .default_value = 0.4f });
    add({ .name = "sync", .max = 3.0f, .default_value = 1.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kFrequencySyncNames, .modulatable = false });
    add({ .name = "tempo", .max = 10.0f, .default_value = 4.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncedFrequencyNames });
    add({ .name = "mix", .default_value = 0.5f });
    add({ .name = "frequency", .min = -6.0f, .max = 3.0f, .default_value = 2.0f, .value_scale = ValueScale::kExponential, .display_invert = true, .decimal_places = 3, .hidden = true });
  }

  std::string getParameterName(int index) override {
    bool is_tempo_or_secs = index == 3;
    if (is_tempo_or_secs) return parameters_[4]->value_processor->value() < 1.0 ? "frequency" : "tempo";
    return Block::getParameterName(index);
  } 

  std::string getParameterName(std::string name) override {
    if (name == "tempo") return parameter_map_["sync"]->value_processor->value() < 1.0 ? "frequency" : "tempo"; 
    return Block::getParameterName(name);
  }
};
}
