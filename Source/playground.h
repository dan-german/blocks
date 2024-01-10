#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/framework/processor.h"
#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/common/synth_constants.h"
#include "vital/synthesis/effects/reverb.h"

using namespace vital;

class NineProcessor: public Processor {
public:
  NineProcessor(): Processor(0, 1) { }
  virtual Processor* clone() const override { return new NineProcessor(*this); }

  void process(int num_samples) override {
    std::cout << "processing" << std::endl;
    auto output_buffer = output(0);
    for (int i = 0; i < num_samples; ++i) {
      output_buffer->buffer[i] = 0.92f;
    }
  }
};

class TwoProcessor: public Processor {
public:
  TwoProcessor(): Processor(0, 1) { }
  virtual Processor* clone() const override { return new TwoProcessor(*this); }

  void process(int num_samples) override {
    std::cout << "processing" << std::endl;
    auto output_buffer = output(0);
    for (int i = 0; i < num_samples; ++i) {
      output_buffer->buffer[i] = 0.2f;
    }
  }
};


class MySynthModule: public SynthModule {
public:
  MySynthModule(): SynthModule(0, 1) {

  }
  virtual SynthModule* clone() const override { return new MySynthModule(*this); }

  void process(int num_samples) override {
    std::cout << "processing" << std::endl;
    auto output_buffer = output(0);
    for (int i = 0; i < num_samples; ++i) {
      output_buffer->buffer[i] = 0.8f;
    }
  }
};

void please() {
  auto reverb = new vital::Reverb();

  // auto value = new vital::Value(0.5f);
  poly_float* buffer = new poly_float[128];


  for (int i = 0; i < 128; ++i) {
    auto factor = 0.5f * ((i % 2 == 0) ? 1.0f : -1.0f);
    buffer[i].set(0, factor);
    buffer[i].set(1, factor);
    buffer[i].set(2, factor);
    buffer[i].set(3, factor);
  }

  reverb->processWithInput(buffer, 128);

  // for (int i = 0; i < 128; ++i) {
  //   std::cout << buffer[i][0] << " " << buffer[i][1] << " " << buffer[i][2] << " " << buffer[i][3] << std::endl;
  // }

  std::cout << "pls" << std::endl;
}