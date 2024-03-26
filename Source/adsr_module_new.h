#pragma once
#include "module_new.h"

namespace model {
class ADSRModule: public Module {
public:
  ADSRModule(int number): Module("envelope", number) {
    display_name = "adsr " + std::to_string(number);
    add({ .name = "attack", .min = 0.0f, .max = 2.37842, .default_value = 0.1495, .value_scale = ValueScale::kQuartic });
    add({ .name = "decay", .max = 2.37842f, .default_value = 1.0f, .value_scale = ValueScale::kQuartic });
    add({ .name = "release", .max = 2.37842f, .default_value = 0.5476f, .value_scale = ValueScale::kQuartic });
    add({ .name = "sustain", .default_value = 1.0f });
  };
};
}