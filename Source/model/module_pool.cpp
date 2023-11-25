/*
  ==============================================================================

    ModulePool.cpp
    Created: 3 Jul 2021 7:35:34pm
    Author:  Dan German

  ==============================================================================
*/

#include "module_pool.h"
#include "oscillator_module_new.h"
#include "filter_module_new.h"
#include "lfo_module_new.h"
// #include "vital/synthesis/modules/oscillator_module.h"
// #include "vital/synthesis/modules/filter_module.h"

namespace model {
ModulePool::~ModulePool() { }

// std::shared_ptr<Tab> ModulePool::getTab(Type type, int number) { return tabs.get({ type, number }); }
std::shared_ptr<Block> ModulePool::GetBlock(std::string type, int number = -1) { return blocks.get({ type, number }); }
void ModulePool::Retire(std::shared_ptr<Block> block) { blocks.retire(block); }
// void ModulePool::Retire(std::shared_ptr<Tab> tab) { tabs.retire(tab); }

void ModulePool::Retire(std::shared_ptr<Module> modulator) {
  colourPool.retire(modulator->colour.id);
  modulators.retire(modulator);
}

ModulePool::ModulePool() {
  // blocks.spawn(Types::blocks, [](std::string type, int number) { return ModuleFactory::createBlock(type, number); });
  // modulators.spawn(Types::modulators, [](std::string type, int number) { return ModuleFactory::createModulator(type, number); });
  // tabs.spawn(Types::tabs, [](std::string type, int number) { return ModuleFactory::createTab(type, number); });
  blocks.spawn({ "osc" }, [](std::string type, int number) { return std::make_shared<model::OscillatorModule>(number); });
  blocks.spawn({ "filter" }, [](std::string type, int number) { return std::make_shared<model::FilterModule>(number); });
  modulators.spawn({ "lfo" }, [](std::string type, int number) { return std::make_shared<model::LFOModule>(number); });

  // for (int i = 1; i <= 40; i++) connections.push_back(std::make_shared<Modulation>(i));

  allModules.insert(allModules.end(), blocks.all.begin(), blocks.all.end());
  allModules.insert(allModules.end(), modulators.all.begin(), modulators.all.end());
  // allModules.insert(allModules.end(), tabs.all.begin(), tabs.all.end());
}

void ModulePool::Retire(std::shared_ptr<model::Modulation> modulationConnection) {
  modulationConnection->reset();
  connections.push_back(modulationConnection);
}

std::shared_ptr<model::Module> ModulePool::GetModulator(std::string code, int number, int colourId) {
  auto modulator = modulators.get({ code, number });
  if (modulator) modulator->colour = colourPool.get(colourId);
  return modulator;
}

std::shared_ptr<Modulation> ModulePool::GetModulation(int number) {
  int index = 0;
  for (int i = 0; i < connections.size(); i++) {
    if (connections[i]->number == number) {
      index = 0;
    }
  }
  auto connection = connections[index];
  connections.erase(connections.begin());
  return connection;
}
} // namespace model