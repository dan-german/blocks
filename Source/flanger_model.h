#pragma once
#include "module_new.h"

namespace model {
class FlangerModule: public Block {
public:
  FlangerModule(int number): Block("flanger", number) {
    add({ .name = "mix", .max = 0.5, .default_value = 0.5, .display_multiply = 200.f });
    add({ .name = "feedback", .min = -1.0f, .default_value = 0.5 });
    add({ .name = "frequency", .min = -5.0f, .max = 2.0f, .value_scale = ValueScale::kExponential, .default_value = 2 });
    add({ .name = "depth", .default_value = 0.5 });
    add({ .name = "offset", .default_value = 0.333333 });
    add({ .name = "center", .min = 8.0, .max = 136.0, .default_value = 64.0 });
  };
};
}

