#include "module_new.h"
// #include "vital/synthesis/modules/lfo_module.h"

// { "cutoff", 0x000000, 8.0, 136.0, 60.0, -60.0, 1.0,
//   ValueDetails::kLinear, false, " semitones", "Cutoff", nullptr },

namespace model {
class ADSRModule: public Module {
// struct ValueDetails {
//   enum ValueScale {
//     kIndexed,
//     kLinear,
//     kQuadratic,
//     kCubic,
//     kQuartic,
//     kSquareRoot,
//     kExponential
//   };

//   std::string name;
//   int version_added = 0;
//   mono_float min = 0.0f;
//   mono_float max = 1.0f;
//   mono_float default_value = 0.0f;
//   // post_offset used to offset quadratic and exponential scaling.
//   mono_float post_offset = 0.0f;
//   mono_float display_multiply = 1.0f;
//   ValueScale value_scale = kLinear;
//   bool display_invert = false;
//   std::string display_units = "";
//   std::string display_name;
//   const std::string* string_lookup = nullptr;
//   std::string local_description;
//   bool audio_rate = false;
//   bool smooth_value = false;
//   bool internal_modulation = false;
//   bool reset = false;
//   Value* val;
//   int decimal_places = 2;
//   bool hidden = false;
//   std::vector<std::string> choices; // equals to string_lookup basically... not the best design choice
// };

public:
  ADSRModule(int number): Module("envelope", number) {
    add({ .name = "attack", .min = 0.0f, .max = 2.37842, .default_value = 0.1495, .value_scale = ValueScale::kQuartic });
    add({ .name = "decay", .max = 2.37842f, .default_value = 1.0f, .value_scale = ValueScale::kQuartic });
    add({ .name = "release", .max = 2.37842f, .default_value = 0.5476f, .value_scale = ValueScale::kQuartic });
    add({ .name = "sustain", .default_value = 1.0f });

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
