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

#include "vital/synthesis/modules/envelope_module.h"

#include "vital/synthesis/modulators/envelope.h"

namespace vital {

EnvelopeModule::EnvelopeModule(bool force_audio_rate):
  SynthModule(kNumInputs, kNumOutputs), prefix_("env"), force_audio_rate_(force_audio_rate) {
  envelope_ = new Envelope();
  envelope_->useInput(input(kTrigger), Envelope::kTrigger);
  envelope_->useOutput(output(kValue), Envelope::kValue);
  envelope_->useOutput(output(kPhase), Envelope::kPhase);
  addProcessor(envelope_);
  setControlRate(!force_audio_rate_);
}

void EnvelopeModule::init() {
  Output* delay = createPolyModControl2({ .name = "delay", .max = 1.41421, .value_scale = ValueScale::kQuadratic });
  Output* attack = createPolyModControl2({ .name = "attack", .max = 2.37842, .default_value = 0.1495, .value_scale = ValueScale::kQuartic });
  Output* hold = createPolyModControl2({ .name = "hold", .max = 1.41421, .value_scale = ValueScale::kQuadratic });
  Output* decay = createPolyModControl2({ .name = "decay", .max = 2.37842, .default_value = 1.0, .value_scale = ValueScale::kQuartic });
  Output* sustain = createPolyModControl2({ .name = "sustain", .default_value = 1.0, });
  Output* release = createPolyModControl2({ .name = "release", .max = 2.37842, .default_value = 0.5476, .value_scale = ValueScale::kQuartic });

  Value* attack_power = createBaseControl2({ .name = "attack_power", .min = -20.0, .max = 20.0 });
  Value* decay_power = createBaseControl2({ .name = "decay_power", .min = -20.0, .max = 20.0, .default_value = -2.0 });
  Value* release_power = createBaseControl2({ .name = "release_power", .min = -20.0, .max = 20.0, .default_value = -2.0, .value_scale = ValueScale::kLinear });

  envelope_->plug(delay, Envelope::kDelay);
  envelope_->plug(attack, Envelope::kAttack);
  envelope_->plug(hold, Envelope::kHold);
  envelope_->plug(decay, Envelope::kDecay);
  envelope_->plug(sustain, Envelope::kSustain);
  envelope_->plug(release, Envelope::kRelease);
  envelope_->plug(attack_power, Envelope::kAttackPower);
  envelope_->plug(decay_power, Envelope::kDecayPower);
  envelope_->plug(release_power, Envelope::kReleasePower);
}

void EnvelopeModule::setControlRate(bool control_rate) {
  // std::string boolybool = control_rate ? "true" : "false";  
  if (!force_audio_rate_)
    envelope_->setControlRate(control_rate);
}
} // namespace vital
