#include "module_new.h"
// #include "vital/synthesis/modules/lfo_module.h"
// #include "vital/synth_strings.h"
#include "vital/synthesis/random_lfo.h"

namespace model {
class RandModel: public Module {
public:
  RandModel(int number): Module("lfo", number) {
    add({ .name = "style", .min = 0.0, .max = RandomLfo::kNumStyles - 1, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncNames });
    add({ .name = "tempo", .min = 12.0, .max = 8.0, .display_name = "rate" }); 
    add({ .name = "sync", .min = 0.0, .max = 4.0, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kFrequencySyncNames });
    add({ .name = "frequency", .min = -7.0, .max = 9.0, .value_scale = ValueScale::kExponential, .display_invert = true, .display_name = "rate", .decimal_places = 3, .hidden = true });
  }
};
}

// const ValueDetails ValueDetailsLookup::random_lfo_parameter_list[] = {
//   { "style", 0x000401, 0.0, RandomLfo::kNumStyles - 1, 0.0, 0.0, 1.0,
//     ValueDetails::kIndexed, false, "", "Style", strings::kRandomNames },
//   { "frequency", 0x000401, -7.0, 9.0, 1.0, 0.0, 1.0,
//     ValueDetails::kExponential, true, " secs", "Frequency", nullptr },
//   { "sync", 0x000401, 0.0, SynthLfo::kNumSyncOptions - 1, 1.0, 0.0, 1.0,
//     ValueDetails::kIndexed, false, "", "Sync", strings::kFrequencySyncNames },
//   { "tempo", 0x000401, 0.0, 12.0, 8.0, 0.0, 1.0,
//     ValueDetails::kIndexed, false, "", "Tempo", strings::kSyncedFrequencyNames },
//   { "stereo", 0x000401, 0.0, 1.0, 0.0, 0.0, 1.0,
//     ValueDetails::kIndexed, false, "", "Stereo", strings::kOffOnNames },
//   { "sync_type", 0x000600, 0.0, 1.0, 0.0, 0.0, 1.0,
//     ValueDetails::kIndexed, false, "", "Sync Type", strings::kOffOnNames },
//   { "keytrack_transpose", 0x000704, -60.0, 36.0, -12.0, 0.0, 1.0,
//     ValueDetails::kIndexed, false, "", "Transpose", nullptr },
//   { "keytrack_tune", 0x000704, -1.0, 1.0, 0.0, 0.0, 100.0,
//     ValueDetails::kLinear, false, "", "Tune", nullptr },
// };
