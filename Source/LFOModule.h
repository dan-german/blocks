/*
  ==============================================================================

    LFOModule.h
    Created: 8 Jun 2021 5:27:45pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once


#include "Module.h"
#include "ModelConstants.h"
#include "NoteHelper.h"
#include "ModuleParameter.h"

namespace Model {
  struct LFOModule: public Module {
    enum Parameters { pWaveform, pSynced, pRate, pMode };

    LFOModule(int number): Module(Types::lfo, number) {
      category = Module::Category::modulator;

      createChoiceParameter({ "wave", waveforms, 1 });

      auto sync = createChoiceParameter({ .name = "sync", .choices = { "hz", "tempo", "dotted", "triplets" }, .defaultIndex = 0 });

      auto range = NormalisableRange(0.001f, 100.0f, 0.0001f);

      std::function<String(double)> textFromValueFunction = [range, sync](double value) {
        if (sync->audioParameter->getValue() == 0) {
          float floatValue = static_cast<float>(value);
          std::stringstream stream;
          stream << std::fixed << std::setprecision(2) << floatValue;
          std::string s = stream.str();
          return String(s);
        }

        auto mappedValue = jmap(float(value), range.start, range.end, 0.0f, 9.0f);
        return NoteHelper::durationStrings[int(mappedValue)];
      };

      createFloatParameter({ .name = "rate", .defaultValue = 1.0f, .textFromValueFunction = textFromValueFunction, .range = range, .valueSuffix = "hz", .skew = 0.2f });
      createChoiceParameter({ .name = "mode", .choices = { "sync", "retrigger" }, .defaultIndex = 0 });
    }

    ~LFOModule() {}
  };
}