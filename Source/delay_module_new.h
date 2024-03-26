#include "module_new.h"
#include "vital/synthesis/effects/delay.h"
#include "vital/synth_strings.h"

namespace model {
class DelayModule: public Block {
public:
  DelayModule(int number): Block("delay", number) {
    add({ .name = "mix" });
    add({ .name = "cutoff", .min = 8.0f, .max = 136.0f, .default_value = 60.0f });
    add({ .name = "style", .max = 3.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kDelayStyleNames });
    add({ .name = "feedback", .min = -1.0f, .default_value = 0.5f });
    add({ .name = "tempo", .max = 12.0f, .default_value = 9.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncedFrequencyNames });
    add({ .name = "sync", .max = 3.0f, .default_value = 1.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kFrequencySyncNames });
    add({ .name = "tempo 2", .max = 12.0f, .default_value = 9.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncedFrequencyNames });
    add({ .name = "sync 2", .max = 3.0f, .default_value = 1.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kFrequencySyncNames });
    add({ .name = "frequency", .min = -2.0f, .max = 9.0f, .default_value = 2.0f, .value_scale = ValueScale::kExponential, .display_invert = true, .decimal_places = 3, .hidden = true });
    add({ .name = "frequency 2", .min = -2.0f, .max = 9.0f, .default_value = 2.0f, .value_scale = ValueScale::kExponential, .display_invert = true, .decimal_places = 3, .hidden = true });
  }

  std::string getParameterName(int index) override {
    bool is_tempo_or_secs_1 = index == 4;
    if (is_tempo_or_secs_1) return parameters_[5]->value_processor->value() < 1.0 ? "frequency" : "tempo";

    bool is_tempo_or_secs_2 = index == 6;
    if (is_tempo_or_secs_2) return parameters_[7]->value_processor->value() < 1.0 ? "frequency 2" : "tempo 2";

    return Block::getParameterName(index);
  }
};
}
