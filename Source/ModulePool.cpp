/*
  ==============================================================================

    ModulePool.cpp
    Created: 3 Jul 2021 7:35:34pm
    Author:  Dan German

  ==============================================================================
*/

#include "ModulePool.h"

ModulePool::~ModulePool() { }

shared_ptr<Tab> ModulePool::getTab(Type type, int number) { return tabs.get({ type, number }); }
shared_ptr<Block> ModulePool::getBlock(Type type, int number = -1) { return blocks.get({ type, number }); }
void ModulePool::retire(shared_ptr<Block> block) { blocks.retire(block); }
void ModulePool::retire(shared_ptr<Tab> tab) { tabs.retire(tab); }

void ModulePool::retire(shared_ptr<Module> modulator) {
  colourPool.retire(modulator->colour.id);
  modulators.retire(modulator);
}

ModulePool::ModulePool() {
  blocks.spawn(Types::blocks, [](Type type, int number) { return ModuleFactory::createBlock(type, number); });
  modulators.spawn(Types::modulators, [](Type type, int number) { return ModuleFactory::createModulator(type, number); });
  tabs.spawn(Types::tabs, [](Type type, int number) { return ModuleFactory::createTab(type, number); });

  for (int i = 1; i <= 40; i++) connections.add(make_shared<Modulation>(i));

  allModules.addArray(blocks.all);
  allModules.addArray(modulators.all);
  allModules.addArray(tabs.all);
}

void ModulePool::retire(shared_ptr<Modulation> modulationConnection) {
  modulationConnection->reset();
  connections.add(modulationConnection);
}

shared_ptr<Module> ModulePool::getModulator(Type code, int number, int colourId) {
  auto modulator = modulators.get({ code, number });
  if (modulator) modulator->colour = colourPool.get(colourId);
  return modulator;
}

shared_ptr<Modulation> ModulePool::getModulation(int number) {
  for (int i = 0; i < connections.size(); i++)
    if (connections[i]->number == number)
      return connections.removeAndReturn(i);

  return connections.removeAndReturn(0);
}
