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

#include "vital/synthesis/modules/lfo_module.h" 

#include "vital/common/line_generator.h"
#include "vital/synthesis/modulators/synth_lfo.h"

namespace vital {

LfoModule::LfoModule(const std::string& prefix, LineGenerator* line_generator, const Output* beats_per_second):
  SynthModule(kNumInputs, kNumOutputs), prefix_(prefix), beats_per_second_(beats_per_second) {
  lfo_ = new SynthLfo(line_generator);
  addProcessor(lfo_);

  setControlRate(true);
}

void LfoModule::init() {
  Output* free_frequency = createPolyModControl2({ .name = "frequency", .min = -7.0f, .max = 9.0f, .value_scale = ValueDetails::kExponential, .default_value = 1.0f });
  Output* phase = createPolyModControl2({ .name = "phase" });
  Output* fade = createPolyModControl2({ .name = "fade_time", .max = 8.0f });
  Output* delay = createPolyModControl2({ .name = "delay_time", .max = 4.0f });
  Output* stereo_phase = createPolyModControl2({ .name = "stereo", .min = -0.5f, .max = 0.5f });
  Value* sync_type = createBaseControl2({ .name = "sync_type", .max = 5.0f, .value_scale = ValueDetails::kIndexed });
  Value* smooth_mode = createBaseControl2({ .name = "smooth_mode", .value_scale = ValueDetails::kIndexed, .default_value = 1.0f });
  Output* smooth_time = createPolyModControl2({ .name = "smooth_time", .min = -10.0f, .max = 4.0f, .value_scale = ValueDetails::kExponential, .default_value = -7.5f });
  Output* frequency = createTempoSyncSwitch(prefix_, free_frequency->owner, beats_per_second_, true, input(kMidi));
  lfo_->useInput(input(kNoteTrigger), SynthLfo::kNoteTrigger);
  lfo_->useInput(input(kNoteCount), SynthLfo::kNoteCount);

  lfo_->useOutput(output(kValue), SynthLfo::kValue);
  lfo_->useOutput(output(kOscPhase), SynthLfo::kOscPhase);
  lfo_->useOutput(output(kOscFrequency), SynthLfo::kOscFrequency);
  lfo_->plug(frequency, SynthLfo::kFrequency);
  lfo_->plug(phase, SynthLfo::kPhase);
  lfo_->plug(stereo_phase, SynthLfo::kStereoPhase);
  lfo_->plug(sync_type, SynthLfo::kSyncType);
  lfo_->plug(smooth_mode, SynthLfo::kSmoothMode);
  lfo_->plug(fade, SynthLfo::kFade);
  lfo_->plug(smooth_time, SynthLfo::kSmoothTime);
  lfo_->plug(delay, SynthLfo::kDelay);
}

void LfoModule::correctToTime(double seconds) {
  lfo_->correctToTime(seconds);
}

void LfoModule::setModule(std::shared_ptr<model::Module> module) {
  SynthModule::setModule(module);

  module->parameter_map_["wave"]->val = control_map_["tempo"];
  module->parameter_map_["tempo"]->val = control_map_["tempo"];
  module->parameter_map_["frequency"]->val = control_map_["frequency"];
  module->parameter_map_["sync"]->val = control_map_["sync"];
  module->parameter_map_["mode"]->val = control_map_["sync_type"];
}

void LfoModule::setControlRate(bool control_rate) {
  Processor::setControlRate(control_rate);
  lfo_->setControlRate(control_rate);
}
} // namespace vital
