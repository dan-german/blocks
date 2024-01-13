#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/framework/processor.h"
#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/common/synth_constants.h"
#include "vital/synthesis/effects/reverb.h"
#include "vital/synthesis/filters/diode_filter.h"

using namespace vital;

class MyProcessor: public Processor {
public:
  float val;
  MyProcessor(float val): Processor(1, 1), val(val) {}
  Processor* clone() const override { return nullptr; }

  void process(int num_samples) override {
    auto buffer = input()->source->buffer;
    for (int i = 0; i < num_samples; ++i) { buffer[i] = val; }
    std::cout << "processing: " << val << std::endl;
  }
};

void please() {
  auto processor = new MyProcessor(0.5f);
  auto processor2 = new MyProcessor(0.6f);
  auto router = new vital::ProcessorRouter(1, 1);
  router->addProcessor(processor);
  router->addProcessor(processor2);
  router->process(128);
}