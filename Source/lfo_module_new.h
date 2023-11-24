#include "module_new.h"
#include "vital/synthesis/modules/lfo_module.h"

// { "cutoff", 0x000000, 8.0, 136.0, 60.0, -60.0, 1.0,
//   ValueDetails::kLinear, false, " semitones", "Cutoff", nullptr },

namespace model {
class LFOModule: public Module {
public:
  LFOModule(int number): Module("lfo", number) {
    add({ .name = "frequency", .min = -7.0, .max = 9.0 });
  }
};
}