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

#include "vital/synthesis/modules/flanger_module.h"

#include "vital/synthesis/lookups/memory.h"
#include "vital/common/synth_constants.h"

namespace vital {

FlangerModule::FlangerModule(const Output* beats_per_second):
  SynthModule(kNumInputs, kNumOutputs), beats_per_second_(beats_per_second),
  frequency_(nullptr), phase_offset_(nullptr), mod_depth_(nullptr),
  phase_(0.0f), delay_(nullptr) { }

FlangerModule::~FlangerModule() { }

void FlangerModule::init() {
  static const cr::Value kDelayStyle(StereoDelay::kClampedUnfiltered);
  delay_frequency_ = new cr::Value(0.0f);

  delay_ = new MultiDelay(kMaxSamples);
  addIdleProcessor(delay_);
  delay_->useOutput(output(kAudioOutput));

  phase_ = 0.0f;

  free_frequency = createPolyModControl2({ .name = "frequency", .value_scale = ValueScale::kExponential, .min = -5.0f, .max = 2.0f, .default_value = 2.0f });
  frequency_ = createTempoSyncSwitch("flanger", free_frequency->owner, beats_per_second_, false);
  center_ = createPolyModControl2({ .name = "center", .min = 8.0f, .max = 136.0f, .default_value = 64.0f, .reset = input(kReset) });
  feedback = createPolyModControl2({ .name = "feedback", .min = -1.0f, .default_value = 0.5f });
  wet = createPolyModControl2({ .name = "mix", .min = 0.0f, .max = 0.5f, .default_value = 0.5f, .reset = input(kReset) });
  mod_depth_ = createPolyModControl2({ .name = "depth",  .default_value = 0.5f });
  phase_offset_ = createPolyModControl2({ .name = "offset",  .default_value = 0.333333f });

  delay_->plug(delay_frequency_, StereoDelay::kFrequency);
  delay_->plug(feedback, StereoDelay::kFeedback);
  delay_->plug(wet, StereoDelay::kWet);
  delay_->plug(&kDelayStyle, StereoDelay::kStyle);

  SynthModule::init();
}

void FlangerModule::process(int num_samples) {
  // utils::print(audio_in[0], "audio_in", this);
  static constexpr float kMaxFrequency = 20000.0f;

  SynthModule::process(num_samples);
  auto audio_in = input()->source->buffer;
  poly_float frequency = frequency_->buffer[0];
  poly_float delta_phase = (frequency * num_samples) / getSampleRate();
  phase_ = utils::mod(phase_ + delta_phase);

  poly_float phase_offset = phase_offset_->buffer[0];
  // poly_float right_offset = (phase_offset);// & constants::kRightMask);
  poly_float phase_total = phase_ - phase_offset / 2.0f + phase_offset;

  poly_float mod = mod_depth_->buffer[0] * (utils::triangleWave(phase_total) * 2.0f - 1.0f) + 1.0f;
  poly_float delay = poly_float(1.0f) / utils::midiNoteToFrequency(center_->buffer[0]);
  delay = (delay - kModulationDelayBuffer) * mod + kModulationDelayBuffer;
  poly_float delay_frequency = poly_float(1.0f) / utils::max(delay, 1.0f / kMaxFrequency);

  output(kFrequencyOutput)->buffer[0] = delay_frequency;
  delay_frequency_->set(delay_frequency);
  delay_->processWithInput(audio_in, num_samples);

  // auto audio_out = output()->buffer;
  // auto delay_out = delay_->output()->buffer;
  // utils::copyBuffer(audio_out, delay_out, num_samples);
  // for (int s = 0; s < num_samples; ++s) {
  //   // poly_float sample_out = ;
  //   audio_out[s] += audio_in[s];// + utils::swapVoices(sample_out);
  //   utils::copyBuffer(
  // }
  // utils::print(audio_out[0], "audio_out", this);
}

void FlangerModule::correctToTime(double seconds) {
  phase_ = utils::getCycleOffsetFromSeconds(seconds, frequency_->buffer[0]);
}
} // namespace vital
