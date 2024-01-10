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

  void setModule(std::shared_ptr<model::Module> module) override { 
    // control_map_["attack"] = module->parameter_map_["attack"]->val;
    // control_map_["decay"] = module->parameter_map_["decay"]->val;
    // control_map_["sustain"] = module->parameter_map_["sustain"]->val;
    // control_map_["release"] = module->parameter_map_["release"]->val;

    module->parameter_map_["attack"]->val = control_map_["attack"];
    module->parameter_map_["decay"]->val = control_map_["decay"];
    module->parameter_map_["sustain"]->val = control_map_["sustain"];
    module->parameter_map_["release"]->val = control_map_["release"];

    // for (auto parameter : module->parameters_) { 
    //   parameter->val->set(parameter->default_value);
    // }

    SynthModule::setModule(module);
  };

  void followModule(std::shared_ptr<model::Module> module) { 
    control_map_["attack"] = module->parameter_map_["attack"]->val;
    control_map_["decay"] = module->parameter_map_["decay"]->val;
    control_map_["sustain"] = module->parameter_map_["sustain"]->val;
    control_map_["release"] = module->parameter_map_["release"]->val;
    auto a = control_map_["attack"];
    // a.
  }

  Envelope* envelope_;
protected:
  std::string prefix_;
  bool force_audio_rate_;

  JUCE_LEAK_DETECTOR(EnvelopeModule)
};
} // namespace vital