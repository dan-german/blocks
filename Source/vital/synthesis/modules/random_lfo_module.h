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

namespace vital {
class RandomLfo;

class RandomLfoModule: public SynthModule {
public:
  enum {
    kNoteTrigger,
    kMidi,
    kNumInputs
  };

  RandomLfoModule(const std::string& prefix, const Output* beats_per_second);
  virtual ~RandomLfoModule() { }

  void init() override;
  virtual Processor* clone() const override { return new RandomLfoModule(*this); }
  void correctToTime(double seconds) override;

  void setModule(std::shared_ptr<model::Module> module) override {
    SynthModule::setModule(module);
    module->parameter_map_["style"]->value_processor = control_map_["style"];
    module->parameter_map_["stereo"]->value_processor = control_map_["stereo"];
    module->parameter_map_["tempo"]->value_processor = control_map_["tempo"];
    module->parameter_map_["frequency"]->value_processor = control_map_["frequency"];
    module->parameter_map_["sync"]->value_processor = control_map_["sync"];
  }

protected:
  std::string prefix_;
  RandomLfo* lfo_;
  const Output* beats_per_second_;

  JUCE_LEAK_DETECTOR(RandomLfoModule)
};
} // namespace vital

