/*
  ==============================================================================

    EnvelopeModule.h
    Created: 5 Jun 2021 12:37:38am
    Author:  Dan German

  ==============================================================================
*/

#include "Module.h"
#include "ModuleParameter.h"

namespace Model {
  struct EnvelopeModule: public Module {
    enum Parameters { attack, decay, sustain, release };
    ~EnvelopeModule() {}

    EnvelopeModule(int number): Module(Types::adsr, number) {
      category = Category::modulator;
      auto secondsRange = NormalisableRange(0.0f, 20.0f, 0.0001f);

      createFloatParameter({ .name = "attack", .defaultValue = 0.0f, .textFromValueFunction = {}, .range = secondsRange,  .valueSuffix = "", .skew = 0.3f });
      createFloatParameter({ .name = "decay", .defaultValue = 0.0f,  .textFromValueFunction = {}, .range = secondsRange, .valueSuffix = "", .skew = 0.3f });
      createFloatParameter({ .name = "sustain", .defaultValue = 1.0f, .skew = 0.3f });
      createFloatParameter({ .name = "release", .defaultValue = 1.0f,  .textFromValueFunction = {}, .range = secondsRange, .valueSuffix = "", .skew = 0.3f });
    }
  };
}