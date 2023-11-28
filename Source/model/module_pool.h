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
#include "module_new.h"
#include "modulation_new.h"

// using namespace Model;
namespace model { 
struct ModulePool {
public:
  ColourPool colourPool;

  ModuleContainer<model::Block> blocks;
  ModuleContainer<model::Module> modulators;
  // ModuleContainer<Tab> tabs;

  std::vector<std::shared_ptr<model::Module>> allModules;
  std::vector<std::shared_ptr<model::Module>> parameterToModuleMap;
  std::vector<std::shared_ptr<model::Modulation>> connections;

  ModulePool();
  ~ModulePool();

  // std::shared_ptr<Tab> getTab(std::string code, int number);
  std::shared_ptr<model::Block> getBlock(std::string code, int number);
  std::shared_ptr<model::Module> getModulator(std::string code, int number, int colourId);
  std::shared_ptr<model::Modulation> getModulation(int number = -1);

  void Retire(std::shared_ptr<model::Modulation> modulationConnection);
  void Retire(std::shared_ptr<model::Block> block);
  void Retire(std::shared_ptr<model::Module> modulator);
  // void retire(std::shared_ptr<Tab> tab);
};
} // namespace model