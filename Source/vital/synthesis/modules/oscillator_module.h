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

#pragma once

#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/producers/synth_oscillator.h"
#include "vital/synthesis/modules/envelope_module.h"
#include "adsr_module_new.h"

namespace vital {
class Wavetable;

class OscillatorModule: public SynthModule {
public:
  enum {
    kReset,
    kRetrigger,
    kMidi,
    kActiveVoices,
    kNumInputs
  };

  enum {
    kRaw,
    kLevelled,
    kNumOutputs
  };

  OscillatorModule();
  virtual ~OscillatorModule() { }

  void process(int num_samples) override;
  void init() override;
  virtual Processor* clone() const override { return new OscillatorModule(*this); }

  Output* amp_env_destination;
  poly_float multiply_;

  Wavetable* getWavetable() { return wavetable_.get(); }
  force_inline SynthOscillator* oscillator() { return oscillator_; }

  SynthOscillator::DistortionType getDistortionType() {
    int val = distortion_type_->value();
    return static_cast<SynthOscillator::DistortionType>(val);
  }

  std::shared_ptr<model::ADSRModule> default_envelope_module_;
  std::shared_ptr<EnvelopeModule> amplitude_envelope_;

  void resetAmpADSR() {
    amplitude_envelope_->setModule(default_envelope_module_);
  }

  void switchLevelEnvelope(EnvelopeModule* envelope) {
    // amp_env_multiply_->plug(envelope->output(), 0);
  }

  Value* on_;
  SynthOscillator* oscillator_;
protected:
  std::string prefix_;
  std::shared_ptr<Wavetable> wavetable_;
  std::shared_ptr<bool> was_on_;

  int i = 0;
  Value* distortion_type_;

  SmoothMultiply2* amp_env_multiply_ = new SmoothMultiply2();
  // Multiply* amp_env_multiply_ = new Multiply();

  JUCE_LEAK_DETECTOR(OscillatorModule)
};
} // namespace vital

