#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/framework/processor.h"
#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/common/synth_constants.h"
#include "vital/synthesis/effects/reverb.h"
#include "vital/synthesis/filters/diode_filter.h"

using namespace vital;

struct A: public SynthModule { 
  A(): SynthModule(2, 2) {}
  void init() override {
    std::cout << "initing A with address " << this << std::endl;
    auto param_a = createPolyModControl2({ .name = "param_a" });
    auto param_b = createPolyModControl2({ .name = "param_b" });
    std::cout << "A: " << this << std::endl;
    std::cout << param_a << std::endl;
    std::cout << param_b << std::endl;
  }
  void process(int num_samples) override {}
};

struct B: public SynthModule { 
  B(): SynthModule(2, 2) {}
  void init() override {
    std::cout << "initing B with address " << this << std::endl;
    auto param_a = createPolyModControl2({ .name = "param_a" });
    std::cout << "B: " << this << std::endl;
    std::cout << param_a << std::endl;
  }
  void process(int num_samples) override {}
};

void please() {
  system("clear");
  auto a = new A();
  a->init();

  auto b = new B();
  b->init();

  auto val = new vital::Value(0.39f);
  auto poly_mod_destinations = a->data_->poly_mod_destinations["param_a"];
  poly_mod_destinations->plugNext(val);

  auto variable_add_ = new vital::VariableAdd(1);

  b->addProcessor(variable_add_);

  variable_add_->plug(a);
  variable_add_->unplug(a);
  
  // osc->init();
  // Processor* amp_env_destination = osc->data_->poly_mod_destinations["amp_env_destination"];
  // amp_env_destination->plugNext(val);
  // amp_env_destination->plug
  std::cout << "pls" << std::endl;
}