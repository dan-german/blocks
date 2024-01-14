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
  system("clear");
  auto lfo = new MyProcessor(0.2f); std::cout << "lfo: " << lfo << std::endl;
  auto osc = new MyProcessor(0.5f); std::cout << "osc: " << osc << std::endl;
  osc->plug(lfo, 0);

  auto osc2 = new MyProcessor(0.7f); std::cout << "osc2: " << osc2 << std::endl;
  osc2->plug(lfo, 0);

  auto router = new vital::ProcessorRouter(1, 1);
  router->addProcessor(osc);
  // router->addProcessor(osc2);

  // auto router2 = new vital::ProcessorRouter(1, 1);
  // router2->addProcessor(osc2);

  // router2->process(128);
  std::cout << "yes" << std::endl;
}