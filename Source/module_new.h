#include "vital/common/synth_parameters.h"
#include "vital/synthesis/producers/synth_oscillator.h"

using ValueScale = vital::ValueDetails::ValueScale;
using Type = vital::ValueDetails::Type;
using Osc = vital::SynthOscillator;

namespace model {
class Module {
public:
  std::string prefix_;
  std::vector<vital::ValueDetails> parameters_;

  Module(std::string prefix): prefix_(prefix) { }

  void add(vital::ValueDetails parameter) {
    parameter.name = prefix_ + "_" + parameter.name;
    parameters_.push_back(parameter);
  }
};


class OscillatorModule: public Module {
  OscillatorModule(): Module("osc") {
    add({ .name = "on", .value_scale = ValueScale::kIndexed, .display_name = "Switch", .type = Type::kBase });
    add({ .name = "midi_track", .value_scale = ValueScale::kIndexed, .type = Type::kBase });
    add({ .name = "smooth_interpolation", .value_scale = ValueScale::kIndexed, .type = Type::kBase });
    add({ .name = "spectral_unison", .value_scale = ValueScale::kIndexed, .type = Type::kBase });
    add({ .name = "stack_style", .max = vital::SynthOscillator::kNumUnisonStackTypes - 1, .value_scale = ValueScale::kIndexed, .type = Type::kBase });
    add({ .name = "transpose_quantize", .max = 8191.0, .value_scale = ValueScale::kLinear, .display_name = "Transpose Quantize", .type = Type::kBase });

    add({ .name = "wave_frame", .max = vital::kNumOscillatorWaveFrames - 1, .value_scale = ValueScale::kLinear, .input_index = Osc::kWaveFrame });
    add({ .name = "transpose", .min = -48.0, .max = 48.0, .audio_rate = true, .reset = true, .input_index = Osc::kTranspose });
    add({ .name = "tune", .min = -1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .audio_rate = true, .reset = true, .input_index = Osc::kTune });
    add({ .name = "unison_voices", .min = 1.0, .max = 16.0, .default_value = 1.0, .value_scale = ValueScale::kIndexed, .input_index = Osc::kUnisonVoices });
    add({ .name = "unison_detune", .max = 10.0, .default_value = 4.472135955, .display_multiply = 100.0, .value_scale = ValueScale::kQuadratic, .input_index = Osc::kUnisonDetune });
    add({ .name = "detune_power", .min = -5.0, .max = 5.0, .default_value = 1.5, .value_scale = ValueScale::kLinear, .input_index = Osc::kDetunePower });
    add({ .name = "detune_range", .max = 48.0, .default_value = 2.0, .value_scale = ValueScale::kLinear, .input_index = Osc::kDetuneRange });
    add({ .name = "level", .default_value = 0.70710678119, .value_scale = ValueScale::kQuadratic, .audio_rate = true, .smooth_value = true, .reset = true, .input_index = Osc::kAmplitude });
    add({ .name = "pan", .min = -1.0, .post_offset = 1.0, .display_multiply = 100.0, .value_scale = ValueScale::kLinear, .display_units = "%", .display_name = "Pan", .input_index = Osc::kPan });
    add({ .name = "phase", .default_value = 0.5, .audio_rate = true, .smooth_value = true, .reset = true, .input_index = Osc::kPhase });
    add({ .name = "distortion_phase", .default_value = 0.5, .input_index = Osc::kDistortionPhase });
    add({ .name = "random_phase", .default_value = 0.0, .input_index = Osc::kRandomPhase });
    add({ .name = "unison_blend", .default_value = 0.8, .value_scale = ValueScale::kLinear, .input_index = Osc::kBlend });
    add({ .name = "stereo_spread", .input_index = Osc::kStereoSpread });
    add({ .name = "frame_spread", .min = -vital::kNumOscillatorWaveFrames / 2, .max = vital::kNumOscillatorWaveFrames / 2, .value_scale = ValueScale::kLinear, .input_index = Osc::kUnisonFrameSpread });
    add({ .name = "distortion_spread", .min = -0.5, .max = 0.5, .input_index = Osc::kUnisonDistortionSpread });
    add({ .name = "distortion_type", .max = vital::SynthOscillator::kNumDistortionTypes - 1, .value_scale = ValueScale::kIndexed, .type = Type::kBase, .input_index = Osc::kDistortionType });
    add({ .name = "distortion_amount", .default_value = 0.5, .input_index = Osc::kDistortionAmount });
    add({ .name = "spectral_morph_spread", .min = -0.5, .max = 0.5, .input_index = Osc::kUnisonSpectralMorphSpread });
    add({ .name = "spectral_morph_type", .max = vital::SynthOscillator::kNumSpectralMorphTypes - 1, .value_scale = ValueScale::kIndexed, .type = Type::kBase, .input_index = Osc::kSpectralMorphType });
    add({ .name = "spectral_morph_amount", .default_value = 0.5, .input_index = Osc::kSpectralMorphAmount });
    // add({ .name = "destination", .max = vital::constants::kNumSourceDestinations + vital::constants::kNumEffects - 1 });
    // add({ .name = "view_2d", .value_scale = ValueScale::kIndexed });
  }

};
}