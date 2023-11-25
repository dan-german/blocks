#include "module_new.h"
#include "vital/synthesis/modules/lfo_module.h"

// { "cutoff", 0x000000, 8.0, 136.0, 60.0, -60.0, 1.0,
//   ValueDetails::kLinear, false, " semitones", "Cutoff", nullptr },

namespace model {
class LFOModule: public Module {
public:
  LFOModule(int number): Module("lfo", number) {
    // enum Parameters { pWaveform, pSynced, pRate, pMode };
    // add({ .name = "wave", .min = -7.0, .max = 9.0 });
    add({ .name = "tempo", .min = -7.0, .max = 12.0 });
    add({ .name = "frequency", .min = -7.0, .max = 9.0 });
    add({ .name = "sync", .min = 0.0, .max = 4.0 });
    add({ .name = "mode", .min = 0.0, .max = 5.0 });
  }
};
}