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

#include "vital/common/synth_constants.h"
#include "vital/synthesis/framework/synth_module.h"

namespace vital {

class Reverb;

class ReverbModule: public SynthModule {
public:
  ReverbModule();
  virtual ~ReverbModule();

  enum { 
    kAudioIn, 
    kReset
  };

  void init() override;
  void hardReset() override;
  void enable(bool enable) override;

  void setSampleRate(int sample_rate) override;
  void processWithInput(const poly_float* audio_in, int num_samples) override;
  Processor* clone() const override;

  void process(int num_samples) override {
    auto buffer = input(0)->source->buffer;
    processWithInput(input(0)->source->buffer, num_samples);
  }

public:
  Reverb* reverb_;
protected:

  JUCE_LEAK_DETECTOR(ReverbModule)
};
} // namespace vital

