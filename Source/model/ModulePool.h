/*
  ==============================================================================

    ModulePool.h
    Created: 3 Jul 2021 7:35:34pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/ModuleFactory.h"
#include "gui/ColourPool.h"
#include "gui/Tab.h"
#include "ModuleContainer.h"

using namespace Model;

struct ModulePool {
public:
  ColourPool colourPool;

  ModuleContainer<Block> blocks;
  ModuleContainer<Module> modulators;
  ModuleContainer<Tab> tabs;

  Array<std::shared_ptr<Module>> allModules;
  Array<std::shared_ptr<Module>> parameterToModuleMap;
  Array<std::shared_ptr<Modulation>> connections;

  ModulePool();
  ~ModulePool();

  std::shared_ptr<Tab> getTab(Type code, int number);
  std::shared_ptr<Block> getBlock(Type code, int number);
  std::shared_ptr<Module> getModulator(Type code, int number, int colourId);
  std::shared_ptr<Modulation> getModulation(int number = -1);

  void retire(std::shared_ptr<Modulation> modulationConnection);
  void retire(std::shared_ptr<Block> block);
  void retire(std::shared_ptr<Module> modulator);
  void retire(std::shared_ptr<Tab> tab);
};
