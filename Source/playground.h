#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/framework/processor.h"
#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/common/synth_constants.h"
#include "vital/synthesis/effects/reverb.h"
#include "vital/synthesis/filters/diode_filter.h"

using namespace vital;

int compactAudio(poly_float* audio_out, const poly_float* audio_in, int num_samples) {
  int num_full = num_samples / 2;
  for (int i = 0; i < num_full; ++i) {
    int in_index = 2 * i;
    audio_out[i] = utils::compactFirstVoices(audio_in[in_index], audio_in[in_index + 1]);
  }

  for (int i = 0; i < num_full; ++i) {
    utils::print(audio_out[i], "ao", nullptr);
  }

  int num_remaining = num_samples % 2;

  if (num_remaining)
    audio_out[num_full] = audio_in[num_samples - 1];

  return num_full + num_remaining;
}

void please() {
  system("clear");

  int size = 5;
  poly_float* a = new poly_float[size];
  poly_float* b = new poly_float[size];

  for (int i = 0; i < size; i++) {
    a[i] = 6.0f;
    b[i] = 9.0f;
  }

  // std::cout << "yay";

  // auto c = utils::compactFirstVoices(a[0], b[0]);

  compactAudio(a, b, size);
}


// i = 0, in_index = 0
// i = 1, in_index = 2
// i = 2, in_index = 4
// i = 3, in_index = 6
// i = 4, in_index = 8

// a, b, 0, 0
// a, b, 0, 0
// a, b, a, b