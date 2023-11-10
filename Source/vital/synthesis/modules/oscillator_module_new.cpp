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

#include "oscillator_module_new.h"

#include "vital/synthesis/producers/synth_oscillator.h"
#include "vital/synthesis/lookups/wavetable.h"

namespace vital {

OscillatorModuleNew::OscillatorModuleNew(std::string prefix):
  SynthModule(kNumInputs, kNumOutputs), prefix_(std::move(prefix)), on_(nullptr), distortion_type_(nullptr) {
  wavetable_ = std::make_shared<Wavetable>(kNumOscillatorWaveFrames);
  was_on_ = std::make_shared<bool>(true);
}

void OscillatorModuleNew::init() {
  DBG("OscillatorModuleNew::init");
  oscillator_ = new SynthOscillator(wavetable_.get());

  createBaseControl(prefix_ + "_view_2d");
  on_ = createBaseControl(prefix_ + "_on");
  Value* midi_track = createBaseControl(prefix_ + "_midi_track");
  Value* smooth_interpolation = createBaseControl(prefix_ + "_smooth_interpolation");
  Value* spectral_unison = createBaseControl(prefix_ + "_spectral_unison");
  Value* stack_style = createBaseControl(prefix_ + "_stack_style");
  Value* transpose_quantize = createBaseControl(prefix_ + "_transpose_quantize");

  Input* reset = input(kReset);

  Output* tune = createPolyModControl(prefix_ + "_tune", true, false, nullptr, reset);

  oscillator_->useInput(input(kReset), SynthOscillator::kReset);
  oscillator_->useInput(input(kRetrigger), SynthOscillator::kRetrigger);
  oscillator_->useInput(input(kActiveVoices), SynthOscillator::kActiveVoices);
  oscillator_->useInput(input(kMidi), SynthOscillator::kMidiNote);
  oscillator_->useOutput(output(kRaw), SynthOscillator::kRaw);
  oscillator_->useOutput(output(kLevelled), SynthOscillator::kLevelled);

  addProcessor(oscillator_);
  SynthModule::init();
}

void OscillatorModuleNew::setModule(model::Module module) {
  for (int i = 0; i < module.parameters_.size(); i++) {
    auto parameter = module.parameters_[i];
    Output* output;
    if (parameter.type == ValueDetails::Type::kBase) {
      createBaseControl2(parameter);
    } else if (parameter.type == ValueDetails::Type::kPolyMod) {
      output = createPolyModControl2(parameter, parameter.reset ? input(kReset) : nullptr);
    }
    oscillator_->plug(output, i);
  }
}

void OscillatorModuleNew::process(int num_samples) {
  bool on = on_->value();

  if (on)
    SynthModule::process(num_samples);
  else if (*was_on_) {
    output(kRaw)->clearBuffer();
    output(kLevelled)->clearBuffer();
  }

  *was_on_ = on;
}
} // namespace vital
