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

#include "vital/synthesis/modules/random_lfo_module.h" 

#include "vital/synthesis/modulators/random_lfo.h"

namespace vital {

RandomLfoModule::RandomLfoModule(const std::string& prefix, const Output* beats_per_second):
  SynthModule(kNumInputs, 1), prefix_(prefix), beats_per_second_(beats_per_second) {
  lfo_ = new RandomLfo();
  addProcessor(lfo_);
}

void RandomLfoModule::init() {
  Output* free_frequency = createPolyModControl2({ .name = "frequency", .value_scale = ValueScale::kExponential, .min = -7.0f, .max = 9.0f, .default_value = 1.0f });
  Value* style = createBaseControl2({ .name = "style", .max = RandomLfo::kNumStyles - 1 });
  Value* stereo = createBaseControl2({ .name = "stereo" });
  Value* sync_type = createBaseControl2({ .name = "sync" });

  Output* frequency = createTempoSyncSwitch(prefix_, free_frequency->owner, beats_per_second_, true, input(kMidi));
  lfo_->useInput(input(kNoteTrigger), RandomLfo::kReset);
  lfo_->useOutput(output());
  lfo_->plug(frequency, RandomLfo::kFrequency);
  lfo_->plug(style, RandomLfo::kStyle);
  lfo_->plug(stereo, RandomLfo::kStereo);
  lfo_->plug(sync_type, RandomLfo::kSync);
}

void RandomLfoModule::correctToTime(double seconds) {
  lfo_->correctToTime(seconds);
}
} // namespace vital
