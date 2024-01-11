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

#include "reverb_module.h"

#include "vital/synthesis/effects/reverb.h"

namespace vital {

ReverbModule::ReverbModule(): SynthModule(1, 1), reverb_(nullptr) {
  DBG("ReverbModule::ReverbModule()");
  reverb_ = new Reverb();
  addProcessor(reverb_);
}

ReverbModule::~ReverbModule() {}

void ReverbModule::init() {
  reverb_->useInput(input());
  reverb_->useOutput(output());

  Output* reverb_decay_time = createPolyModControl2({ .name = "reverb_decay_time", .min = -6.0, .max = 6.0, .value_scale = ValueScale::kExponential });
  Output* reverb_pre_low_cutoff = createPolyModControl2({ .name = "reverb_pre_low_cutoff", .max = 128.0 });
  Output* reverb_pre_high_cutoff = createPolyModControl2({ .name = "reverb_pre_high_cutoff", .max = 128.0, .default_value = 110.0 });
  Output* reverb_low_shelf_cutoff = createPolyModControl2({ .name = "reverb_low_shelf_cutoff", .max = 128.0 });
  Output* reverb_low_shelf_gain = createPolyModControl2({ .name = "reverb_low_shelf_gain", .min = -6.0, .max = 0.0 });
  Output* reverb_high_shelf_cutoff = createPolyModControl2({ .name = "reverb_high_shelf_cutoff", .max = 128.0, .default_value = 90.0 });
  Output* reverb_high_shelf_gain = createPolyModControl2({ .name = "reverb_high_shelf_gain", .min = -6.0, .max = 0.0, .default_value = -1.0 });
  Output* reverb_chorus_amount = createPolyModControl2({ .name = "reverb_chorus_amount", .min = -8.0, .max = 3.0, .default_value = 0.223607 ,.value_scale = ValueScale::kExponential });
  Output* reverb_chorus_frequency = createPolyModControl2({ .name = "reverb_chorus_frequency", .min = -8.0, .max = 3.0, .default_value = -2.0f, .value_scale = ValueScale::kExponential });
  Output* reverb_size = createPolyModControl2({ .name = "reverb_size" });
  Output* reverb_delay = createPolyModControl2({ .name = "reverb_delay", .max = 0.3 });
  Output* reverb_wet = createPolyModControl2({ .name = "reverb_dry_wet" });

  reverb_->plug(reverb_decay_time, Reverb::kDecayTime);
  reverb_->plug(reverb_pre_low_cutoff, Reverb::kPreLowCutoff);
  reverb_->plug(reverb_pre_high_cutoff, Reverb::kPreHighCutoff);
  reverb_->plug(reverb_low_shelf_cutoff, Reverb::kLowCutoff);
  reverb_->plug(reverb_low_shelf_gain, Reverb::kLowGain);
  reverb_->plug(reverb_high_shelf_cutoff, Reverb::kHighCutoff);
  reverb_->plug(reverb_high_shelf_gain, Reverb::kHighGain);
  reverb_->plug(reverb_chorus_amount, Reverb::kChorusAmount);
  reverb_->plug(reverb_chorus_frequency, Reverb::kChorusFrequency);
  reverb_->plug(reverb_delay, Reverb::kDelay);
  reverb_->plug(reverb_size, Reverb::kSize);
  reverb_->plug(reverb_wet, Reverb::kWet);

  SynthModule::init();
}

void ReverbModule::hardReset() {
  reverb_->hardReset();
}

void ReverbModule::enable(bool enable) {
  SynthModule::enable(enable);
  process(1);
  if (!enable)
    reverb_->hardReset();
}

void ReverbModule::setSampleRate(int sample_rate) {
  SynthModule::setSampleRate(sample_rate);
  reverb_->setSampleRate(sample_rate);
}

void ReverbModule::processWithInput(const poly_float* audio_in, int num_samples) {
  SynthModule::process(num_samples);
  // reverb_->processWithInput(audio_in, num_samples);
}

Processor* ReverbModule::clone() const {
  return new ReverbModule(*this);
}
} // namespace vital
