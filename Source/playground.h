#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/framework/processor.h"
#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/common/synth_constants.h"
#include "vital/synthesis/effects/reverb.h"
#include "vital/synthesis/filters/diode_filter.h"

using namespace vital;

void please() {
  auto reverb = new vital::Reverb();
  poly_float* buffer = new poly_float[100];
  reverb->processWithInput(buffer, 128);
}
