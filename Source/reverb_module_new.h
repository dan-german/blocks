#include "module_new.h"

namespace model {
class ReverbModule: public Block {
public:
  ReverbModule(int number): Block("reverb", number) {
    add({ .name = "mix", .default_value = 1.0, .display_multiply = 100.0, .display_units = "%" });
    add({ .name = "time", .min = -6.0, .max = 6.0, .value_scale = ValueScale::kExponential, .display_units = "s" });
    add({ .name = "size", .default_value = 0.5, .display_multiply = 100.0, .display_units = "%" });
    add({ .name = "delay", .max = 0.3, .default_value = 0.0, .display_units = "s" });
    add({ .name = "high cut", .max = 128.0, .default_value = 110.0 });
    add({ .name = "low cut", .max = 128.0, .default_value = 0.0 });
    add({ .name = "mod rate", .min = -8.0, .max = 3.0, .default_value = -2.0, .value_scale = ValueScale::kExponential, .display_units = "hz" });
    add({ .name = "mod amount", .default_value = 0.223607, .value_scale = ValueScale::kQuadratic, .display_multiply = 100.0, .display_units = "%" });
  }
};
}
