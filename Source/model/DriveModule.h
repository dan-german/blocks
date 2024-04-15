/*
  ==============================================================================

    DriveModule.h
    Created: 23 Jul 2021 6:15:27pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/Block.h"
#include "model/ModelConstants.h"
#include "model/ModuleParameter.h"

namespace Model {
struct DriveModule: public Block {
  enum Parameters { pType, pDrive };

  DriveModule(int number): Block(Model::Types::drive, number) {
    category = Module::Category::effect;
    createChoiceParameter({ "type", { "soft", "hard" }, 0 });
    createFloatParameter({ .name = "drive", .defaultValue = 0.0f, .range = { 0.0f, 1.0f, 0.01f } });
  }

  ~DriveModule() {}
};
}