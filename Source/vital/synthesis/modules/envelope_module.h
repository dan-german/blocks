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
#include "adsr_module_new.h"

namespace vital {
class Envelope;

class EnvelopeModule: public SynthModule {
public:
  enum {
    kTrigger,
    kNumInputs
  };

  enum {
    kValue,
    kPhase,
    kNumOutputs
  };

  EnvelopeModule(bool force_audio_rate = false);
  virtual ~EnvelopeModule() { }

  void init() override;
  virtual Processor* clone() const override { return new EnvelopeModule(*this); }

  void setControlRate(bool control_rate) override;
  // void setControlRate(bool control_rate) override { 
  //   if (!force_audio_rate_)
  //     envelope_->setControlRate(control_rate);
  // }

  void setModule(std::shared_ptr<model::Module> module) { // this should be setProcessor in model::Module?
    module->parameter_map_["attack"]->val = control_map_["attack"];
    module->parameter_map_["decay"]->val = control_map_["decay"];
    module->parameter_map_["sustain"]->val = control_map_["sustain"];
    module->parameter_map_["release"]->val = control_map_["release"];
  };

protected:
  std::string prefix_;
  Envelope* envelope_;
  bool force_audio_rate_;

  JUCE_LEAK_DETECTOR(EnvelopeModule)
};
} // namespace vital