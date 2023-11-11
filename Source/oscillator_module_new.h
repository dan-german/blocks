#include "module_new.h"

namespace model {
class OscillatorModule: public Module {
public:
  OscillatorModule(int number): Module("osc " + std::to_string(number)) {
    add({ .name = "transpose", .min = -48.0, .max = 48.0, .audio_rate = true, .reset = true, .input_index = Osc::kTranspose });
    add({ .name = "tune", .min = -1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .audio_rate = true, .reset = true, .input_index = Osc::kTune });
    add({ .name = "unison_voices", .min = 1.0, .max = 16.0, .default_value = 1.0, .value_scale = ValueScale::kIndexed, .input_index = Osc::kUnisonVoices });
    add({ .name = "unison_detune", .max = 10.0, .default_value = 4.472135955, .display_multiply = 100.0, .value_scale = ValueScale::kQuadratic, .input_index = Osc::kUnisonDetune });
    add({ .name = "level", .default_value = 0.70710678119, .value_scale = ValueScale::kQuadratic, .audio_rate = true, .smooth_value = true, .reset = true, .input_index = Osc::kAmplitude });
    add({ .name = "pan", .min = -1.0, .post_offset = 1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .display_units = "%", .display_name = "Pan", .input_index = Osc::kPan });
  }
};
}