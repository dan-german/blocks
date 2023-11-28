#include "module_new.h"
// #include "vital/synthesis/modules/lfo_module.h"

// { "cutoff", 0x000000, 8.0, 136.0, 60.0, -60.0, 1.0,
//   ValueDetails::kLinear, false, " semitones", "Cutoff", nullptr },

namespace model {
class ADSRModule: public Module {
public:
  ADSRModule(int number): Module("adsr", number) {
    // add({ .name = "delay", .min = -7.0, .max = 12.0 });
    add({ .name = "attack", .min = -7.0, .max = 12.0 });
    // add({ .name = "hold", .min = -7.0, .max = 12.0 });
    add({ .name = "decay", .min = -7.0, .max = 9.0 });
    add({ .name = "release", .min = 0.0, .max = 4.0 });
    add({ .name = "sustain", .min = 0.0, .max = 5.0 });
    // add({ .name = "attack_power", .min = 0.0, .max = 5.0 });
    // add({ .name = "decay_power", .min = 0.0, .max = 5.0 });
    // add({ .name = "release_power", .min = 0.0, .max = 5.0 });
  };
};
}

// const ValueDetails ValueDetailsLookup::env_parameter_list[] = {
//   { "delay", 0x000503, 0.0, 1.4142135624, 0.0, 0.0, 1.0,
//     ValueDetails::kQuartic, false, " secs", "Delay", nullptr },
//   { "attack", 0x000000, 0.0, 2.37842, 0.1495, 0.0, 1.0,
//     ValueDetails::kQuartic, false, " secs", "Attack", nullptr },
//   { "hold", 0x000504, 0.0, 1.4142135624, 0.0, 0.0, 1.0,
//     ValueDetails::kQuartic, false, " secs", "Hold", nullptr },
//   { "decay", 0x000000, 0.0, 2.37842, 1.0, 0.0, 1.0,
//     ValueDetails::kQuartic, false, " secs", "Decay", nullptr },
//   { "release", 0x000000, 0.0, 2.37842, 0.5476, 0.0, 1.0,
//     ValueDetails::kQuartic, false, " secs", "Release", nullptr },
//   { "attack_power", 0x000000, -20.0, 20.0, 0.0f, 0.0, 1.0,
//     ValueDetails::kLinear, false, "", "Attack Power", nullptr },
//   { "decay_power", 0x000000, -20.0, 20.0, -2.0f, 0.0, 1.0,
//     ValueDetails::kLinear, false, "", "Decay Power", nullptr },
//   { "release_power", 0x000000, -20.0, 20.0, -2.0f, 0.0, 1.0,
//     ValueDetails::kLinear, false, "", "Release Power", nullptr },
//   { "sustain", 0x000000, 0.0, 1.0, 1.0, 0.0, 1.0,
//     ValueDetails::kLinear, false, "", "Sustain", nullptr },
// };
