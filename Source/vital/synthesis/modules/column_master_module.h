#pragma once

#include "vital/common/synth_constants.h"
#include "vital/synthesis/framework/synth_module.h"

namespace vital {

class ColumnMasterModule: public SynthModule {
public:

  enum {
    kAudioIn,
    kReset,
    kPan,
    kLevel,
    kNumInputs
  };

  ColumnMasterModule();
  virtual ~ColumnMasterModule();
  void init() override;
  // void setSampleRate(int sample_rate) override;
  // void processWithInput(const poly_float* audio_in, int num_samples) override;
  void process(int num_samples) override;
  Processor* clone() const override { return new ColumnMasterModule(*this); }
protected:
  poly_float level_;
  poly_float pan_;
  JUCE_LEAK_DETECTOR(ColumnMasterModule)
};

} // namespace vital

