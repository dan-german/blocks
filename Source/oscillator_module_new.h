#include "module_new.h"
#include "vital/synthesis/producers/synth_oscillator.h"
#include "vital/common/synth_constants.h"

using Osc = vital::SynthOscillator;

namespace model {
class OscillatorModule: public Block {
public:
  OscillatorModule(int number): Block("osc", number) {
    add({ .name = "wave", .min = 0.0, .max = vital::kNumOscillatorWaveFrames - 1, .reset = true });
    add({ .name = "transpose", .min = -48.0, .max = 48.0, .value_scale = ValueScale::kIndexed });
    add({ .name = "tune", .min = -1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .audio_rate = true, .reset = true });
    add({ .name = "unison_voices", .min = 1.0, .max = 16.0, .default_value = 1.0, .value_scale = ValueScale::kIndexed, .display_name = "stack" });
    add({ .name = "unison_detune", .max = 10.0, .default_value = 4.472135955, .display_multiply = 100.0, .value_scale = ValueScale::kQuadratic, .display_name = "spread" });
    add({ .name = "level", .default_value = 0.70710678119, .value_scale = ValueScale::kQuadratic });
    add({ .name = "pan", .min = -1.0, .post_offset = 1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .display_units = "%", .display_name = "pan" });
  }
};
}