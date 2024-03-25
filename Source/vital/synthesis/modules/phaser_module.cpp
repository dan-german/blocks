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

#include "vital/synthesis/modules/phaser_module.h"
#include "vital/synthesis/effects/phaser.h" 

namespace vital {

PhaserModule::PhaserModule(const Output* beats_per_second):
  SynthModule(1, kNumOutputs), beats_per_second_(beats_per_second), phaser_(nullptr) {
  phaser_ = new Phaser();
}

PhaserModule::~PhaserModule() {
  delete phaser_;
}

void PhaserModule::init() {
  phaser_->useInput(input());
  phaser_->useOutput(output(kAudioOutput), Phaser::kAudioOutput);
  phaser_->useOutput(output(kCutoffOutput), Phaser::kCutoffOutput);

  Output* phaser_free_frequency = createPolyModControl2({ .name = "frequency", .value_scale = ValueScale::kExponential, .min = -5.0f, .max = 2.0f, .default_value = -3.0f, });
  Output* phaser_frequency = createTempoSyncSwitch("phaser", phaser_free_frequency->owner, beats_per_second_, false);
  Output* phaser_feedback = createPolyModControl2({ .name = "feedback", .default_value = 0.5 });
  Output* phaser_wet = createPolyModControl2({ .name = "mix", .default_value = 1.0f });
  Output* phaser_center = createPolyModControl2({ .name = "center", .audio_rate = true, .smooth_value = true, .min = 8.0f, .max = 136.0f, .default_value = 80.0 });
  Output* phaser_mod_depth = createPolyModControl2({ .name = "depth", .max = 48.0f, .default_value = 24.0f });
  Output* phaser_phase_offset = createPolyModControl2({ .name = "offset", .default_value = 0.333333f });
  Output* phaser_blend = createPolyModControl2({ .name = "blend", .max = 2.0f, .default_value = 1.0f });

  phaser_->plug(phaser_frequency, Phaser::kRate);
  phaser_->plug(phaser_wet, Phaser::kMix);
  phaser_->plug(phaser_feedback, Phaser::kFeedbackGain);
  phaser_->plug(phaser_center, Phaser::kCenter);
  phaser_->plug(phaser_mod_depth, Phaser::kModDepth);
  phaser_->plug(phaser_phase_offset, Phaser::kPhaseOffset);
  phaser_->plug(phaser_blend, Phaser::kBlend);
  phaser_->init();
  enable(true);

  SynthModule::init();
}

void PhaserModule::hardReset() {
  phaser_->hardReset();
}

void PhaserModule::enable(bool enable) {
  SynthModule::enable(enable);
  process(1);
  if (enable)
    phaser_->hardReset();
}

void PhaserModule::correctToTime(double seconds) {
  SynthModule::correctToTime(seconds);
  phaser_->correctToTime(seconds);
}

void PhaserModule::setSampleRate(int sample_rate) {
  SynthModule::setSampleRate(sample_rate);
  phaser_->setSampleRate(sample_rate);
}

void PhaserModule::process(int num_samples) {
  SynthModule::process(num_samples);
  poly_float* audio_in = input()->source->buffer;
  phaser_->processWithInput(audio_in, num_samples);
}

Processor* PhaserModule::clone() const {
  auto copy = new PhaserModule(*this);
  copy->phaser_ = static_cast<Phaser*>(copy->phaser_->clone());
  return copy;
}
} // namespace vital