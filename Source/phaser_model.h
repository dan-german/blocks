#pragma once
#include "module_new.h"

namespace model {
class PhaserModule: public Block {
public:
  PhaserModule(int number): Block("phaser", number) {
    add({ .name = "mix", .default_value = 1 });
    add({ .name = "feedback", .default_value = 0.5 });
    add({ .name = "frequency", .min = -5, .max = 2, .value_scale = ValueScale::kExponential, .default_value = -3 });
    add({ .name = "depth", .max = 48, .default_value = 24 });
    add({ .name = "offset", .default_value = 0.333333 });
    add({ .name = "center", .min = 8.0f, .max = 136.0f, .default_value = 80.0f });
    add({ .name = "blend", .max = 2, .default_value = 1 });
  };
};
}
