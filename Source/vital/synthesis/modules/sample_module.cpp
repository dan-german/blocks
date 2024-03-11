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

#include "sample_module.h"

#include "vital/common/synth_constants.h"

namespace vital {

SampleModule::SampleModule(): SynthModule(kNumInputs, kNumOutputs), on_(nullptr) {
  sampler_ = new SampleSource();
  was_on_ = std::make_shared<bool>(true);
}

void SampleModule::init() {
  on_ = createBaseControl2({ .name = "on", .default_value = 1.0 });
  Value* random_phase = createBaseControl2({ .name = "random phase", .default_value = 1.0 });
  Value* loop = createBaseControl2({ .name = "loop", .default_value = 1.0 });
  Value* bounce = createBaseControl2({ .name = "bounce", .default_value = 1.0 });
  Value* keytrack = createBaseControl2({ .name = "keytrack", .default_value = 0.0 });
  Value* transpose_quantize = createBaseControl2({ .name = "transpose quantize" });
  Output* transpose = createPolyModControl2({ .name = "transpose" });
  Output* tune = createPolyModControl2({ .name = "tune" });
  Output* level = createPolyModControl2({ .name = "level", .default_value = 1.0, .audio_rate = true, .smooth_value = true, .value_scale = ValueScale::kQuadratic });
  Output* pan = createPolyModControl2({ .name = "pan", .min = -1.0f });

  sampler_->useInput(input(kReset), SampleSource::kReset);
  sampler_->useInput(input(kMidi), SampleSource::kMidi);
  sampler_->useInput(input(kNoteCount), SampleSource::kNoteCount);
  sampler_->plug(random_phase, SampleSource::kRandomPhase);
  sampler_->plug(keytrack, SampleSource::kKeytrack);
  sampler_->plug(loop, SampleSource::kLoop);
  sampler_->plug(bounce, SampleSource::kBounce);
  sampler_->plug(transpose, SampleSource::kTranspose);
  sampler_->plug(transpose_quantize, SampleSource::kTransposeQuantize);
  sampler_->plug(tune, SampleSource::kTune);
  sampler_->plug(level, SampleSource::kLevel);
  sampler_->plug(pan, SampleSource::kPan);
  // sampler_->useOutput(output(kRaw), SampleSource::kRaw);
  // sampler_->useOutput(output(kLevelled), SampleSource::kLevelled);

  addProcessor(sampler_);
  amp_env_destination = createPolyModControl2({ .name = "amp env destination", .reset = input(kReset) });
  addProcessor(amp_env_multiply_);
  amp_env_multiply_->plug(amp_env_destination, 1);
  amp_env_multiply_->plug(sampler_->output(kLevelled), 0);
  amp_env_multiply_->useInput(input(kReset), SmoothMultiply2::kReset);
  amp_env_multiply_->useOutput(output(kRaw), 0);

  SynthModule::init();
}

void SampleModule::process(int num_samples) {
  bool on = on_->value();
  if (on)
    SynthModule::process(num_samples);
  else if (*was_on_) {
    output(kRaw)->clearBuffer();
    output(kLevelled)->clearBuffer();
    getPhaseOutput()->buffer[0] = 0.0f;
  }
  utils::addBuffers(output(kRaw)->buffer, output(kRaw)->buffer, input(kAudioIn)->source->buffer, num_samples);
  *was_on_ = on;
}
} // namespace vital
