#pragma once
#include "module_new.h"

namespace model {
class NoiseModel: public Block {
public:
  NoiseModel(int number): Block("noise", number) {
    add({ .name = "level" });
    add({ .name = "pan", .value_scale = ValueScale::kQuadratic });
  };
};
}