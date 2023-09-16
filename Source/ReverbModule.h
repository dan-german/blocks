/*
  ==============================================================================

    ReverbModule.h
    Created: 3 Jun 2021 6:28:13pm
    Author:  Dan German

  ==============================================================================
*/

#include "Block.h"
#include "ModelConstants.h"
#include "ModuleParameter.h"

namespace Model {
  struct ReverbModule: public Block {
    enum Parameters { pSize, pDamping, pWidth, pMix };
    ~ReverbModule() {}

    ReverbModule(int number): Block(Types::reverb, number) {
      category = Module::Category::effect;
      auto range = NormalisableRange(0.0f, 1.0f, 0.001f);
      createFloatParameter({ .name = "size", .defaultValue = 0.5f });
      createFloatParameter({ .name = "damping", .defaultValue = 0.5f });
      createFloatParameter({ .name = "width", .defaultValue = 0.5f });
      createFloatParameter({ .name = "mix", .defaultValue = 0.3f });
    }
  };
}