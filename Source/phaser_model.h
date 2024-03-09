#pragma once
#include "module_new.h"

namespace model {
class PhaserModule: public Block {
public:
  PhaserModule(int number): Block("phaser", number) {
    add({ .name = "mix", .default_value = 1.0 });
    add({ .name = "feedback", .default_value = 0.5 });
    add({ .name = "depth", .max = 48, .default_value = 24 });
    add({ .name = "tempo", .max = 10.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncedFrequencyNames });
    add({ .name = "sync", .max = 3.0f, .value_scale = ValueScale::kIndexed, .default_value = 1.0f, .string_lookup = strings::kFrequencySyncNames });
    add({ .name = "offset", .default_value = 0.333333 });
    add({ .name = "center", .min = 8.0f, .max = 136.0f, .default_value = 80.0f });
    add({ .name = "blend", .max = 2.0, .default_value = 1.0 });
    add({ .name = "frequency", .min = -5.0f, .max = 2.0f, .value_scale = ValueScale::kExponential, .display_invert = true, .decimal_places = 3, .default_value = 2.0f, .hidden = true });
  };
};
}