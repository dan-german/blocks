#include "module_new.h"
#include "vital/synthesis/modules/lfo_module.h"
#include "vital/synth_strings.h"

namespace model {
class LFOModule: public Module {
public:
  LFOModule(int number): Module("lfo", number) {
    add({ .name = "wave", .min = 0.0, .max = 4.0, .value_scale = ValueScale::kIndexed });
    add({ .name = "tempo", .min = -7.0, .max = 12.0, .display_name = "rate" });
    add({ .name = "sync", .min = 0.0, .max = 4.0, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kFrequencySyncNames });
    add({ .name = "mode", .min = 0.0, .max = 5.0, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kSyncNames });
    add({ .name = "frequency", .min = -7.0, .max = 9.0, .display_name = "rate", .hidden = true });
  }
};
}