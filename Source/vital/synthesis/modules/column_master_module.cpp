#include "vital/synthesis/modules/column_master_module.h"

namespace vital {

ColumnMasterModule::ColumnMasterModule(): SynthModule(kNumInputs, 1) {}

ColumnMasterModule::~ColumnMasterModule() {}

void ColumnMasterModule::init() {
  Output* pan = createPolyModControl2({ .name = "pan", .min = -1.0 });
  Output* level = createPolyModControl2({ .name = "level", .default_value = 1.0 });
  // plug(pan, kPan);
  plug(pan, kPan);
  plug(level, kLevel);
  SynthModule::init();
}

void ColumnMasterModule::process(int num_samples) {
  SynthModule::process(num_samples);
  poly_float* audio_in = input()->source->buffer;
  poly_float* audio_out = output()->buffer;

  utils::copyBuffer(audio_out, audio_in, num_samples);

  poly_float current_pan = pan_;
  
  pan_ = futils::panAmplitude(utils::clamp(input(kPan)->at(0), -1.0f, 1.0f));
  current_pan = utils::maskLoad(current_pan, pan_, getResetMask(kReset));
  poly_float delta_pan = (pan_ - current_pan) * (1.0f / num_samples);

  const poly_float* amplitude = input(kLevel)->source->buffer;
  poly_float amp = utils::max(amplitude[0], 0.0f);
  poly_float zero = 0.0f;
  for (int i = 0; i < num_samples; ++i) {
    current_pan += delta_pan;
    audio_out[i] = current_pan * audio_in[i] * amp * amp;
    VITAL_ASSERT(utils::isFinite(audio_out[i]));
  }
}
} // namespace vital