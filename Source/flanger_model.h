#pragma once
#include "module_new.h"

namespace model {
class FlangerModule: public Block {
public:
  FlangerModule(int number): Block("flanger", number) {
    add({ .name = "mix", .max = 0.5, .default_value = 0.5, .display_multiply = 200.0 }); 
    add({ .name = "feedback", .min = -1.0, .default_value = 0.5 }); 
    add({ .name = "depth", .default_value = 0.5 });
    add({ .name = "sync", .max = 3.0, .default_value = 4.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kFrequencySyncNames, .modulatable = false }); 
    add({ .name = "tempo", .max = 10.0, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncedFrequencyNames });
    add({ .name = "offset", .default_value = 0.333333 });
    add({ .name = "center", .min = 8.0, .max = 136.0, .default_value = 64.0 });
    add({ .name = "frequency", .min = -5.0, .max = 2.0, .default_value = 2.0, .value_scale = ValueScale::kExponential, .display_invert = true, .decimal_places = 3, .hidden = true });
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