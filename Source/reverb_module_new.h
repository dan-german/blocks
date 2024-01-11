#include "module_new.h"
// #include "vital/synthesis/rev/synth_filter.h"

// { "cutoff", 0x000000, 8.0, 136.0, 60.0, -60.0, 1.0,
//   ValueDetails::kLinear, false, " semitones", "Cutoff", nullptr },

namespace model {
class ReverbModule: public Block {
public:
  ReverbModule(int number): Block("reverb", number) {
    add({ .name = "chorus_frequency", .min = -8.0, .max = 3.0, .default_value = -2.0, .value_scale = ValueScale::kExponential });
    add({ .name = "decay_time", .min = -6.0, .max = 6.0, .value_scale = ValueScale::kExponential });
    add({ .name = "dry_wet", .default_value = 1.0 });
    add({ .name = "high_shelf_cutoff", .max = 128.0, .default_value = 90.0 });
    add({ .name = "high_shelf_gain", .min = -6.0, .max = 0.0, .default_value = -1.0 });
    add({ .name = "low_shelf_cutoff", .max = 128.0, .default_value = 0.0 });
    add({ .name = "low_shelf_gain", .min = -6.0, .max = 0.0, .default_value = 0.0 });
    // add({ .name = "on", .default_value = 1.0 });
    add({ .name = "pre_high_cutoff", .max = 128.0, .default_value = 110.0 });
    add({ .name = "pre_low_cutoff", .max = 128.0, .default_value = 0.0 });
    add({ .name = "size", .default_value = 0.5 });
    add({ .name = "delay", .max = 0.3, .default_value = 0.0 });
  }
};
}

// reverb: [
// {
// name: 'chorus_frequency',
// min : -8,
// max : 3,
// value_scale : 6,
// default_value : -2
// },
//     {
//       name: 'decay_time',
//       min : -6,
//       max : 6,
//       value_scale : 6,
//       default_value : 0
//     },
// { name: 'dry_wet', default_value : 1 },
// { name: 'high_shelf_cutoff', max : 128, default_value : 90 },
// { name: 'high_shelf_gain', min : -6, max : 0, default_value : -1 },
// { name: 'low_shelf_cutoff', max : 128, default_value : 0 },
// { name: 'low_shelf_gain', min : -6, max : 0, default_value : 0 },
// { name: 'on', value_scale : 0, default_value : 1 },
// { name: 'pre_high_cutoff', max : 128, default_value : 110 },
// { name: 'pre_low_cutoff', max : 128, default_value : 0 },
// { name: 'size', default_value : 0.5 },
// { name: 'delay', max : 0.3, default_value : 0 }
// ]