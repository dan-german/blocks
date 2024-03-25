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

int counterino = 0;

ReverbModule::ReverbModule(): SynthModule(2, 1), reverb_(nullptr) {
  // DBG("ReverbModule::ReverbModule()");
  reverb_ = new Reverb();
  addProcessor(reverb_);
}

ReverbModule::~ReverbModule() {}

void ReverbModule::init() {
  reverb_->useInput(input());
  reverb_->useOutput(output());
  reverb_->useInput(input(kReset), Reverb::kReset);

  Output* reverb_decay_time = createMonoModControl2({ .name = "time", .value_scale = ValueScale::kExponential, .min = -6.0, .max = 6.0 });
  Output* reverb_pre_low_cutoff = createMonoModControl2({ .name = "low cut", .max = 128.0 });
  Output* reverb_pre_high_cutoff = createMonoModControl2({ .name = "high cut", .max = 128.0, .default_value = 110.0 });
  Output* reverb_low_shelf_cutoff = createMonoModControl2({ .name = "reverb_low_shelf_cutoff", .max = 128.0 });
  Output* reverb_low_shelf_gain = createMonoModControl2({ .name = "reverb_low_shelf_gain", .min = -6.0, .max = 0.0 });
  Output* reverb_high_shelf_cutoff = createMonoModControl2({ .name = "reverb_high_shelf_cutoff", .max = 128.0, .default_value = 90.0 });
  Output* reverb_high_shelf_gain = createMonoModControl2({ .name = "reverb_high_shelf_gain", .min = -6.0, .max = 0.0, .default_value = -1.0 });
  Output* reverb_chorus_amount = createMonoModControl2({ .name = "mod amount", .value_scale = ValueScale::kQuadratic , .default_value = 0.223607 });
  Output* reverb_chorus_frequency = createMonoModControl2({ .name = "mod rate", .value_scale = ValueScale::kExponential , .min = -8.0, .max = 3.0, .default_value = -2.0 });
  Output* reverb_size = createMonoModControl2({ .name = "size", .min = 0.0, .max = 1.0, .default_value = 0.5 });
  Output* reverb_delay = createMonoModControl2({ .name = "delay", .max = 0.3 });
  Output* reverb_wet = createMonoModControl2({ .name = "mix" });

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
//  process(1);
  if (!enable)
    reverb_->hardReset();
}

void ReverbModule::setSampleRate(int sample_rate) {
  SynthModule::setSampleRate(sample_rate);
  reverb_->setSampleRate(sample_rate);
}


void ReverbModule::processWithInput(const poly_float* audio_in, int num_samples) {
  SynthModule::process(num_samples);
}

Processor* ReverbModule::clone() const {
  return new ReverbModule(*this);
}
} // namespace vital
