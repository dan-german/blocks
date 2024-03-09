/* Copyright 2013-2019 Matt Tytel
 *
 * vital is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * vital is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with vital.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "oscillator_module.h"

#include "vital/synthesis/producers/synth_oscillator.h"
#include "vital/synthesis/lookups/wavetable.h"

namespace vital {

OscillatorModule::OscillatorModule():
  SynthModule(kNumInputs, kNumOutputs), prefix_(std::move("osc")), on_(nullptr), distortion_type_(nullptr) {
  wavetable_ = std::make_shared<Wavetable>(kNumOscillatorWaveFrames);
  was_on_ = std::make_shared<bool>(true);
}

void OscillatorModule::init() {
  oscillator_ = new SynthOscillator(wavetable_.get());

  createBaseControl2({ .name = "view_2d" });
  on_ = createBaseControl2({ .name = "on" });
  Value* midi_track = createBaseControl2({ .name = "midi_track", .value_scale = ValueScale::kIndexed, .default_value = 1.0 });
  Value* smooth_interpolation = createBaseControl2({ .name = "smooth_interpolation", .value_scale = ValueScale::kIndexed });
  Value* spectral_unison = createBaseControl2({ .name = "spectral_unison", .value_scale = ValueScale::kIndexed, .default_value = 1.0 });
  Value* stack_style = createBaseControl2({ .name = "stack_style", .max = 10.0, .value_scale = ValueScale::kIndexed });
  Value* transpose_quantize = createBaseControl2({ .name = "transpose_quantize", .max = 8191.0, .value_scale = ValueScale::kIndexed });

  Input* reset = input(kReset);

  Output* wave_frame = createPolyModControl2({ .name = "wave", .max = 3.0, .value_scale = ValueScale::kIndexed });
  Output* transpose = createPolyModControl2({ .name = "transpose", .audio_rate = true, .reset = reset, .min = -48, .max = 48, .value_scale = ValueScale::kIndexed });
  Output* tune = createPolyModControl2({ .name = "tune", .audio_rate = true, .reset = reset, .max = 1.0 });
  Output* unison_voices = createPolyModControl2({ .name = "unison voices", .min = 1.0, .max = 16.0, .value_scale = ValueScale::kIndexed, .default_value = 1.0 });
  Output* unison_detune = createPolyModControl2({ .name = "unison detune", .value_scale = ValueScale::kQuadratic, .default_value = 2.2360679775 });
  Output* detune_power = createPolyModControl2({ .name = "detune power" });
  Output* detune_range = createPolyModControl2({ .name = "detune range", .max = 48, .default_value = 2.0 });
  Output* amplitude = createPolyModControl2({ .name = "level", .audio_rate = true, .smooth_value = true, .reset = reset, .default_value = 0.70710678119 });
  Output* pan = createPolyModControl2({ .name = "pan", .min = -1.0 });
  Output* phase = createPolyModControl2({ .name = "phase", .audio_rate = true, .smooth_value = true, .reset = reset, .default_value = 0.5 });
  Output* distortion_phase = createPolyModControl2({ .name = "distortion phase", .default_value = 0.5 });
  Output* rand_phase = createPolyModControl2({ .name = "random phase", .default_value = 1.0 });
  Output* blend = createPolyModControl2({ .name = "unison blend", .default_value = 0.8 });
  Output* stereo_spread = createPolyModControl2({ .name = "stereo spread", .default_value = 1.0 });
  Output* frame_spread = createPolyModControl2({ .name = "frame spread", .min = -128.0, .max = 128.0 });
  Output* distortion_spread = createPolyModControl2({ .name = "distortion spread", .min = -0.5, .max = 0.5 });
  distortion_type_ = createBaseControl2({ .name = "distortion type", .max = 12.0, .value_scale = ValueScale::kIndexed });
  Output* distortion_amount = createPolyModControl2({ .name = "distortion amount", .default_value = 0.5 });
  Output* spectral_morph_spread = createPolyModControl2({ .name = "spectral morph spread", .min = -0.5, .max = 0.5 });
  Value* spectral_morph_type = createBaseControl2({ .name = "spectral morph type", .max = 11.0, .value_scale = ValueScale::kIndexed });
  Output* spectral_morph_amount = createPolyModControl2({ .name = "spectral morph amount", .default_value = 0.5 });

  oscillator_->useInput(input(kReset), SynthOscillator::kReset);
  oscillator_->useInput(input(kRetrigger), SynthOscillator::kRetrigger);
  oscillator_->useInput(input(kActiveVoices), SynthOscillator::kActiveVoices);
  oscillator_->useInput(input(kMidi), SynthOscillator::kMidiNote);
  oscillator_->plug(wave_frame, SynthOscillator::kWaveFrame);
  oscillator_->plug(midi_track, SynthOscillator::kMidiTrack);
  oscillator_->plug(smooth_interpolation, SynthOscillator::kSmoothlyInterpolate);
  oscillator_->plug(spectral_unison, SynthOscillator::kSpectralUnison);
  oscillator_->plug(transpose_quantize, SynthOscillator::kTransposeQuantize);
  oscillator_->plug(transpose, SynthOscillator::kTranspose);
  oscillator_->plug(tune, SynthOscillator::kTune);
  oscillator_->plug(stack_style, SynthOscillator::kStackStyle);
  oscillator_->plug(unison_detune, SynthOscillator::kUnisonDetune);
  oscillator_->plug(unison_voices, SynthOscillator::kUnisonVoices);
  oscillator_->plug(phase, SynthOscillator::kPhase);
  oscillator_->plug(distortion_phase, SynthOscillator::kDistortionPhase);
  oscillator_->plug(rand_phase, SynthOscillator::kRandomPhase);
  oscillator_->plug(blend, SynthOscillator::kBlend);
  oscillator_->plug(amplitude, SynthOscillator::kAmplitude);
  oscillator_->plug(pan, SynthOscillator::kPan);
  oscillator_->plug(detune_power, SynthOscillator::kDetunePower);
  oscillator_->plug(detune_range, SynthOscillator::kDetuneRange);
  oscillator_->plug(stereo_spread, SynthOscillator::kStereoSpread);
  oscillator_->plug(frame_spread, SynthOscillator::kUnisonFrameSpread);
  oscillator_->plug(distortion_spread, SynthOscillator::kUnisonDistortionSpread);
  oscillator_->plug(distortion_type_, SynthOscillator::kDistortionType);
  oscillator_->plug(distortion_amount, SynthOscillator::kDistortionAmount);
  oscillator_->plug(spectral_morph_spread, SynthOscillator::kUnisonSpectralMorphSpread);
  oscillator_->plug(spectral_morph_type, SynthOscillator::kSpectralMorphType);
  oscillator_->plug(spectral_morph_amount, SynthOscillator::kSpectralMorphAmount);

  amp_env_destination = createPolyModControl2({ .name = "amp env destination", .reset = reset });

  addProcessor(oscillator_);
  addProcessor(amp_env_multiply_);

  amp_env_multiply_->plug(amp_env_destination, 1);
  amp_env_multiply_->plug(oscillator_, 0);
  amp_env_multiply_->useInput(reset, SmoothMultiply2::kReset);
  amp_env_multiply_->useOutput(output(kRaw), 0);
  SynthModule::init();
}

void OscillatorModule::process(int num_samples) {

  bool on = on_->value();
  if (on)
    SynthModule::process(num_samples);
  else if (*was_on_) {
    output(kRaw)->clearBuffer();
    output(kLevelled)->clearBuffer();
  }

  utils::addBuffers(output(kRaw)->buffer, output(kRaw)->buffer, input(kAudioIn)->source->buffer, num_samples);

  *was_on_ = on;
}
} // namespace vital