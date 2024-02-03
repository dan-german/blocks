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

#include "distortion_module.h"

#include "vital/synthesis/effects/distortion.h"
#include "vital/synthesis/filters/digital_svf.h"

namespace vital {

DistortionModule::DistortionModule():
  SynthModule(kNumInputs, 1), distortion_(new Distortion()), mix_(0.0f) {
  addProcessor(distortion_);
}

DistortionModule::~DistortionModule() { }

void DistortionModule::init() {
  distortion_->useOutput(output());

  Value* distortion_type = createBaseControl2({ .name = "type" });
  Output* distortion_drive = createPolyModControl2({ .name = "drive", .audio_rate = true, .smooth_value = true, .reset = input(kReset) });
  distortion_mix_ = createPolyModControl2({ .name = "mix" });
  distortion_->plug(distortion_type, Distortion::kType);
  distortion_->plug(distortion_drive, Distortion::kDrive);

  SynthModule::init();
}

void DistortionModule::setSampleRate(int sample_rate) {
  SynthModule::setSampleRate(sample_rate);
  distortion_->setSampleRate(sample_rate);
}

void DistortionModule::process(int num_samples) {
  SynthModule::process(num_samples);
  // bool on = on_ == nullptr || on_->value() > 0.5f;
  // if (on) {
  const poly_float* audio_in = input()->source->buffer;
  distortion_->processWithInput(audio_in, num_samples);
  poly_float current_mix = mix_;
  mix_ = utils::clamp(distortion_mix_->buffer[0], 0.0f, 1.0f);
  poly_float delta_mix = (mix_ - current_mix) * (1.0f / num_samples);
  poly_float* audio_out = output()->buffer;

  for (int i = 0; i < num_samples; ++i) {
    current_mix += delta_mix;
    audio_out[i] = utils::interpolate(audio_in[i], audio_out[i], current_mix);
  }
  // } else {
  //   utils::zeroBuffer(output()->buffer, num_samples);
  // }
}
} // namespace vital
