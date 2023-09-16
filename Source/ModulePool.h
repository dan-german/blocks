/*
  ==============================================================================

    ModulePool.h
    Created: 3 Jul 2021 7:35:34pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "ModuleFactory.h"
#include "ColourPool.h"
#include "Tab.h"
#include "ModuleContainer.h"

using namespace Model;
using namespace std;

struct ModulePool {
public:
  ColourPool colourPool;

  ModuleContainer<Block> blocks;
  ModuleContainer<Module> modulators;
  ModuleContainer<Tab> tabs;

  Array<shared_ptr<Module>> allModules;
  Array<shared_ptr<Module>> parameterToModuleMap;
  Array<shared_ptr<Modulation>> connections;

  ModulePool();
  ~ModulePool();

  shared_ptr<Tab> getTab(Type code, int number);
  shared_ptr<Block> getBlock(Type code, int number);
  shared_ptr<Module> getModulator(Type code, int number, int colourId);
  shared_ptr<Modulation> getModulation(int number = -1);

  void retire(shared_ptr<Modulation> modulationConnection);
  void retire(shared_ptr<Block> block);
  void retire(shared_ptr<Module> modulator);
  void retire(shared_ptr<Tab> tab);
};
