#include "vital/synthesis/framework/operators.h"
#include "vital/synthesis/framework/processor.h"
#include "vital/synthesis/framework/synth_module.h"
#include "vital/synthesis/utilities/smooth_value.h"
#include "vital/common/synth_constants.h"
#include "vital/synthesis/effects/reverb.h"
#include "vital/synthesis/filters/diode_filter.h"

using namespace vital;


void fillBuffer(poly_float* buffer, int size, float val) {
  for (int i = 0; i < size; i++) {
    buffer[i].set(0, val);
    buffer[i].set(1, val);
    buffer[i].set(2, val);
    buffer[i].set(3, val);
  }
}

void playFirstVoice(vital::Output* reset, vital::DiodeFilter* diode);

void playSecondVoice(vital::Output* reset, vital::DiodeFilter* diode);

void please() {
  auto diode = new vital::DiodeFilter();
  auto audio = new vital::Output();
  auto reset = new vital::Output();

  auto drive = new vital::Output();
  fillBuffer(drive->buffer, 128, 0.1f);

  diode->plug(audio, vital::SynthFilter::kAudio);
  diode->plug(reset, vital::SynthFilter::kReset);
  diode->plug(drive, vital::SynthFilter::kDriveGain);

  playFirstVoice(reset, diode);

  reset->clearTrigger();
  fillBuffer(drive->buffer, 128, 0.7f);

  playSecondVoice(reset, diode);

  reset->clearTrigger();
  fillBuffer(drive->buffer, 128, 0.35f);

  playFirstVoice(reset, diode);
}

void playSecondVoice(vital::Output* reset, vital::DiodeFilter* diode) {
  std::cout << "playing second" << std::endl;
  poly_mask second_mask = constants::kSecondMask;
  reset->trigger(second_mask, kVoiceOn, 0);
  diode->process(128);
}

void playFirstVoice(vital::Output* reset, vital::DiodeFilter* diode) {
  std::cout << "playing first" << std::endl;
  poly_mask first_mask = constants::kFirstMask;
  reset->trigger(first_mask, kVoiceOn, 0);
  diode->process(128);
}