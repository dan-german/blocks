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

#include "delay_module.h"

#include "vital/synthesis/effects/delay.h"
#include "vital/synthesis/lookups/memory.h"

namespace vital {

DelayModule::DelayModule(const Output* beats_per_second): SynthModule(2, 1), beats_per_second_(beats_per_second) {
  int size = kMaxDelayTime * getSampleRate();
  delay_ = new MultiDelay(size);
  addProcessor(delay_);
}

DelayModule::~DelayModule() { }

void DelayModule::init() {
  delay_->useInput(input());
  delay_->useOutput(output());
  delay_->useInput(input(kReset), MultiDelay::kReset);

  // { "delay_tempo", 0x000000, 4.0, 12.0, 9.0, 0.0, 1.0, ValueDetails::kIndexed, false, "", "Delay Tempo", strings::kSyncedFrequencyNames },
  AddControlInput tempo_input = { .name = "tempo", .min = -2.0f, .max = 9.0f, .default_value = 9.0f, .value_scale = ValueScale::kIndexed };
  Output* free_frequency = createPolyModControl2({ .name = "frequency", .min = -2.0f, .max = 9.0f, .value_scale = ValueScale::kExponential, .default_value = 1.4f });
  Output* frequency = createTempoSyncSwitch2(tempo_input, free_frequency->owner, beats_per_second_, true);

  Output* free_frequency_aux = createPolyModControl2({ .name = "frequency 2", .min = -2.0f, .max = 9.0f, .value_scale = ValueScale::kExponential, .default_value = 1.4f });
  AddControlInput aux_tempo_input = { .name = "tempo 2", .min = -2.0f, .max = 9.0f, .default_value = 9.0f, .value_scale = ValueScale::kIndexed };
  Output* frequency_aux = createTempoSyncSwitch2(aux_tempo_input, free_frequency_aux->owner, beats_per_second_, true, nullptr, "sync 2");

  Output* feedback = createPolyModControl2({ .name = "feedback", .min = -1.0f, .default_value = 0.5f });
  Output* wet = createPolyModControl2({ .name = "mix",  .default_value = 0.5f });

  Output* filter_cutoff = createPolyModControl2({ .name = "cutoff", .min = 8.0f, .max = 136.0f, .default_value = 60.0f });
  Output* filter_spread = createPolyModControl2({ .name = "spread", .default_value = 1.0f });
  Value* style = createBaseControl2({ .name = "style", .max = 3.0f, .value_scale = ValueScale::kIndexed });

  delay_->plug(frequency, StereoDelay::kFrequency);
  delay_->plug(frequency_aux, StereoDelay::kFrequencyAux);
  delay_->plug(feedback, StereoDelay::kFeedback);
  delay_->plug(wet, StereoDelay::kWet);
  delay_->plug(style, StereoDelay::kStyle);
  delay_->plug(filter_cutoff, StereoDelay::kFilterCutoff);
  delay_->plug(filter_spread, StereoDelay::kFilterSpread);

  SynthModule::init();
}

void DelayModule::setSampleRate(int sample_rate) {
  SynthModule::setSampleRate(sample_rate);
  delay_->setSampleRate(sample_rate);
  delay_->setMaxSamples(kMaxDelayTime * getSampleRate());
}

void DelayModule::setOversampleAmount(int oversample) {
  SynthModule::setOversampleAmount(oversample);
  delay_->setMaxSamples(kMaxDelayTime * getSampleRate());
}

int i = 0;

void DelayModule::processWithInput(const poly_float* audio_in, int num_samples) {
  SynthModule::process(num_samples);
  // delay_->processWithInput(audio_in, num_samples);

  // if (i++ % 100 == 0) {
  //   utils::print(audio_in[0], "de: ", this);
  // }
// 
  // i++;
}
} // namespace vital