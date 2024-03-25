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

#include "vital/synthesis/framework/processor_router.h"
#include "vital/synthesis/filters/phaser_filter.h"
#include "vital/synthesis/framework/operators.h"

namespace vital {

class PhaserFilter;

class Phaser: public ProcessorRouter {
public:
  enum {
    kAudio,
    kMix,
    kRate,
    kFeedbackGain,
    kCenter,
    kModDepth,
    kPhaseOffset,
    kBlend,
    kNumInputs
  };

  enum {
    kAudioOutput,
    kCutoffOutput,
    kNumOutputs
  };

  Phaser();
  virtual ~Phaser() {
    std::cout << "removing phaser\n";
    // removeProcessor(phaser_filter_);
    delete phaser_filter_;
    delete cutoff_;
  }

  virtual Processor* clone() const override {
    auto p = new Phaser(*this);
    std::cout << "first filter: " << p->phaser_filter_ << std::endl;
    p->phaser_filter_ = static_cast<PhaserFilter*>(phaser_filter_->clone());
    p->cutoff_ = new Output();
    std::cout << "second filter: " << p->phaser_filter_ << std::endl;
    // p->addIdleProcessor(p->phaser_filter_);
    p->phaser_filter_->useInput(p->input(kFeedbackGain), PhaserFilter::kResonance);
    p->phaser_filter_->useInput(p->input(kBlend), PhaserFilter::kPassBlend);
    p->phaser_filter_->plug(p->cutoff_, PhaserFilter::kMidiCutoff);
    return p;
  }

  void process(int num_samples) override;
  void processWithInput(const poly_float* audio_in, int num_samples) override;
  void init() override;
  void hardReset() override;
  void correctToTime(double seconds);
  void setOversampleAmount(int oversample) override {
    ProcessorRouter::setOversampleAmount(oversample);
    cutoff_->ensureBufferSize(oversample * kMaxBufferSize);
  }

  Output* cutoff_;
  PhaserFilter* phaser_filter_;

private:
  poly_float mix_;
  poly_float mod_depth_;
  poly_float phase_offset_;
  poly_int phase_;

  JUCE_LEAK_DETECTOR(Phaser)
};
} // namespace vital