/*
  ==============================================================================

    Filter.h
    Created: 14 Jul 2021 4:54:05pm
    Author:  Dan German

  =================================================================== ===========
*/

#pragma once

#include "Module.h"
#include "ModelConstants.h"
#include "ModuleParameter.h"

namespace Model {
  struct FilterModule : public Block {
    enum Parameters { pType, pFrequency, pQ };
    ~FilterModule() {}

    FilterModule(int number) : Block(Types::filter, number) {
      category = Module::Category::effect;
      StringArray filterTypes{ "LP4", "LP2", "HP2", "HP4", "BP2", "BP4" }; 

      createChoiceParameter({ "type", filterTypes, 0 });
      createFloatParameter({ .name = "cutoff", .defaultValue = 440.0f, .range = { 20.0f, 20480.0f, 0.01f }, .valueSuffix = "hz", .skew = 0.35f });
      createFloatParameter({ .name = "q", .defaultValue = 0.0f });
    }
  };
}