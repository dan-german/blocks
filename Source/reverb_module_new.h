#include "module_new.h"
// #include "vital/synthesis/rev/synth_filter.h"

// { "cutoff", 0x000000, 8.0, 136.0, 60.0, -60.0, 1.0,
//   ValueDetails::kLinear, false, " semitones", "Cutoff", nullptr },

namespace model {
class ReverbModule: public Block {
public:
  ReverbModule(int number): Block("reverb", number) {
    add({ .name = "mix", .default_value = 1.0, .display_units = "%", .display_multiply = 100.0 });
    add({ .name = "time", .min = -6.0, .max = 6.0, .value_scale = ValueScale::kExponential, .display_units = "s" });
    add({ .name = "size", .default_value = 0.5, .display_units = "%", .display_multiply = 100.0 });
    add({ .name = "delay", .max = 0.3, .default_value = 0.0, .display_units = "s" });
    add({ .name = "high cut", .max = 128.0, .default_value = 110.0 });
    add({ .name = "low cut", .max = 128.0, .default_value = 0.0 });
    add({ .name = "mod rate", .min = -8.0, .max = 3.0, .default_value = -2.0, .value_scale = ValueScale::kExponential, .display_units = "hz" });
    add({ .name = "mod amount", .default_value = 0.223607, .value_scale = ValueScale::kQuadratic, .display_units = "%", .display_multiply = 100.0 });

    // add({ .name = "high_shelf_cutoff", .max = 128.0, .default_value = 90.0, .hidden = true });
    // add({ .name = "high_shelf_gain", .min = -6.0, .max = 0.0, .default_value = -1.0, .hidden = true });
    // add({ .name = "low_shelf_cutoff", .max = 128.0, .default_value = 0.0, .hidden = true });
    // add({ .name = "low_shelf_gain", .min = -6.0, .max = 0.0, .default_value = 0.0, .hidden = true });
  }
};
}