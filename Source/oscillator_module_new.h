#include "module_new.h"
#include "vital/synthesis/producers/synth_oscillator.h"
#include "vital/common/synth_constants.h"
#include "vital/synth_strings.h"

using Osc = vital::SynthOscillator;

namespace model {
class OscillatorModule: public Block {
public:
  OscillatorModule(int number): Block("osc", number) {
    add({ .name = "wave", .min = 0.0, .max = 3.0, .value_scale = ValueScale::kIndexed, .string_lookup = strings::waves });
    add({ .name = "transpose", .min = -48.0, .max = 48.0, .value_scale = ValueScale::kIndexed });
    add({ .name = "tune", .min = -1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .audio_rate = true, .reset = true });
    add({ .name = "unison voices", .min = 1.0, .max = 16.0, .default_value = 1.0, .value_scale = ValueScale::kIndexed, .display_name = "stack" });
    add({ .name = "unison detune", .max = 10.0, .default_value = 2.2360679775, .display_multiply = 100.0, .value_scale = ValueScale::kQuadratic, .display_name = "spread" });
    add({ .name = "level", .default_value = 0.70710678119, .value_scale = ValueScale::kQuadratic });
    add({ .name = "pan", .min = -1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .display_units = "%", .display_name = "pan" });
    add({ .name = "amp env destination", .hidden = true });
  }
};
}