#include "ModuleManager.h"

ModuleManager::~ModuleManager() {
  clear();
}

std::shared_ptr<Block> ModuleManager::addBlock(Model::Type code, Index index, int number) {
  auto block = pool.getBlock(code, number);
  if (block == nullptr) return nullptr;
  block->index = index;
  nameToModuleMap[block->name] = block;
  blockMatrix[index.row][index.column] = block;
  blocks.add(block);
  return block;
}

void ModuleManager::removeBlock(std::shared_ptr<Block> block) {
  for (auto connection : getConnectionsOfTarget(block))
    removeConnection(connection);

  nameToModuleMap.erase(block->name);
  blockMatrix[block->index.row][block->index.column] = nullptr;
  blocks.remove(blocks.indexOf(block));
  pool.retire(block);
}

std::shared_ptr<Module> ModuleManager::addModulator(Model::Type code, int number, int colourId) {
  auto modulator = pool.getModulator(code, number, colourId);
  if (modulator == nullptr) { return nullptr; }
  modulators.add(modulator);
  nameToModuleMap[modulator->name] = modulator;
  return modulator;
}

Array<std::shared_ptr<Modulation>> ModuleManager::getConnectionsOfSource(std::shared_ptr<Module> source) {
  Array<std::shared_ptr<Modulation>> sourceConnections;

  for (auto connection : connections)
    if (connection->source == source)
      sourceConnections.add(connection);

  return sourceConnections;
}

Array<std::shared_ptr<Modulation>> ModuleManager::getConnectionsOfTarget(std::shared_ptr<Module> target) {
  Array<std::shared_ptr<Modulation>> targetConnections;

  for (auto connection : connections)
    if (connection->target == target)
      targetConnections.add(connection);

  return targetConnections;
}

void ModuleManager::removeModulator(int index) {
  auto modulator = modulators.removeAndReturn(index);

  for (auto connection : getConnectionsOfSource(modulator))
    removeConnection(connection);

  pool.retire(modulator);
  nameToModuleMap.erase(modulator->name);
}

void ModuleManager::repositionBlock(Index oldIndex, Index newIndex) {
  auto block = getBlock(oldIndex);

  block->index = newIndex;

  blockMatrix[newIndex.row][newIndex.column] = block;
  blockMatrix[oldIndex.row][oldIndex.column] = {};
}

std::shared_ptr<Modulation> ModuleManager::addConnection(std::shared_ptr<Module> source, std::shared_ptr<Module> target, int parameterIndex, int number) {
  if (connectionExists(parameterIndex, source, target)) return nullptr;

  auto connection = pool.getModulation(number);
  connection->parameterIndex = parameterIndex;
  connection->source = source;
  connection->target = target;
  connections.add(connection);
  target->parameters[parameterIndex]->connections.add(connection);
  return connection;
}

bool ModuleManager::connectionExists(int parameterIndex, std::shared_ptr<Module> source, std::shared_ptr<Module> target) {
  for (auto connection : connections) {
    bool connectionExists = connection->target == target
      && connection->source == source
      && parameterIndex == connection->parameterIndex;

    if (connectionExists) return true;
  }
  return false;
}

void ModuleManager::removeConnection(int index) {
  auto connection = connections.removeAndReturn(index);
  connection->target->removeConnection(connection);
  pool.retire(connection);
}

void ModuleManager::removeConnection(std::shared_ptr<Modulation> connection) {
  connection->target->removeConnection(connection);
  pool.retire(connections.removeAndReturn(connections.indexOf(connection)));
}

Array<std::shared_ptr<Modulation>> ModuleManager::getConnections() {
  Array<std::shared_ptr<Modulation>> array;
  array.addArray(connections);
  return array;
}

Array<std::shared_ptr<Module>> ModuleManager::getModulators() {
  Array<std::shared_ptr<Module>> array;
  for (auto object : modulators) array.add(object);
  return array;
}

void ModuleManager::clear() {
  for (int i = tabs.size() - 1; i >= 0; i--) removeTab(tabs[i]);
  for (int i = blocks.size() - 1; i >= 0; i--) removeBlock(blocks[i]);
  for (int i = connections.size() - 1; i >= 0; i--) removeConnection(i);
  for (int i = modulators.size() - 1; i >= 0; i--) removeModulator(i);
}

void ModuleManager::removeTab(std::shared_ptr<Tab> tab) {
  for (auto connection : getConnectionsOfTarget(tab))
    removeConnection(connection);

  pool.retire(tab);
  tabs.removeFirstMatchingValue(tab);
  nameToModuleMap.erase(tab->name);
}

void ModuleManager::triggerNoteInTabs(Voice* voice) {
  voice->setAll(true);
  for (auto tab : getTabs()) {
    voice->setColumnsState(tab->getAllColumns(), false);
    voice->setColumnsState(tab->getNextColumns(static_cast<int>(tab->parameter(static_cast<int>(Tab::Parameters::columns))->getNormalizedValue())), true);
  }
}

Array<int> ModuleManager::getActiveColumns() {
  Array<int> activeColumns;

  for (auto tab : tabs)
    activeColumns.addArray(tab->activeColumns);

  return activeColumns;
}

std::shared_ptr<Tab> ModuleManager::addTab(Model::Type type, int column, int number) {
  auto tab = pool.getTab(type, number);
  if (tab == nullptr) return nullptr;
  tab->column = column;
  nameToModuleMap[tab->name] = tab;
  tabs.add(tab);
  return tab;
}

std::shared_ptr<Tab> ModuleManager::getTab(int column) {
  for (auto tab : tabs) {
    if (tab->column == column) return tab;
  }

  return nullptr;
}