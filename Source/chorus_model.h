#pragma once
#include "module_new.h"
// #include "vital/synthesis/effects/distortion.h"

namespace model {
class ChorusModule: public Block {
public:
  ChorusModule(int number): Block("chorus", number) {
    add({ .name = "voices", .min = 1, .max = 4, .value_scale = ValueScale::kIndexed, .display_multiply = 4.0f });
    add({ .name = "depth", .default_value = 0.5f });
    add({ .name = "feedback", .min = -0.95f, .max = 0.95f, .default_value = 0.4f });
    add({ .name = "tempo", .min = 0.0f, .max = 10.0f, .value_scale = ValueScale::kIndexed, .default_value = 4.0f, .string_lookup = strings::kSyncedFrequencyNames });
    add({ .name = "sync", .max = 3.0f, .value_scale = ValueScale::kIndexed, .default_value = 1.0f, .string_lookup = strings::kFrequencySyncNames });
    add({ .name = "mix", .default_value = 0.5f });
    add({ .name = "frequency", .min = -6.0f, .max = 3.0f, .value_scale = ValueScale::kExponential, .display_invert = true, .decimal_places = 3, .default_value = 2.0f, .hidden = true });
  };
};
}

// chorus: [
//     {
//       name: 'delay_1',
//       min: -10,
//       max: -5.64386,
//       value_scale: 6,
//       default_value: -9
//     },
//     {
//       name: 'delay_2',
//       min: -10,
//       max: -5.64386,
//       value_scale: 6,
//       default_value: -7
//     },
//     { name: 'dry_wet', default_value: 0.5 },
//     { name: 'feedback', min: -0.95, max: 0.95, default_value: 0.4 },
//     {
//       name: 'frequency',
//       min: -6,
//       max: 3,
//       value_scale: 6,
//       default_value: -3
//     },
//     { name: 'mod_depth', default_value: 0.5 },
//     { name: 'on', value_scale: 0, default_value: 0 },
//     { name: 'sync', max: 3, value_scale: 0, default_value: 1 },
//     { name: 'tempo', max: 10, value_scale: 0, default_value: 4 },
//     {
//       name: 'voices',
//       min: 1,
//       max: 4,
//       value_scale: 0,
//       default_value: 4
//     },
//     { name: 'spread', default_value: 1 }
//   ],