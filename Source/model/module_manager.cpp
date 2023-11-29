#include "model/module_manager.h"

namespace model {
ModuleManager::~ModuleManager() {
  clear();
}

std::shared_ptr<model::Block> ModuleManager::addBlock(std::string code, Index index, int number) {
  auto block = pool.getBlock(code, number);
  if (block == nullptr) return nullptr;
  block->index = index;
  nameToModuleMap[block->name] = block;
  blockMatrix[index.row][index.column] = block;
  blocks.push_back(block);
  return block;
}

void ModuleManager::removeBlock(std::shared_ptr<model::Block> block) {
  for (auto connection : getConnectionsOfTarget(block)) {
    removeConnection(connection);
  }
  nameToModuleMap.erase(block->name);
  blockMatrix[block->index.row][block->index.column] = nullptr;
  blocks.erase(std::remove(blocks.begin(), blocks.end(), block), blocks.end());
  pool.Retire(block);
}

std::shared_ptr<Module> ModuleManager::addModulator(std::string code, int number, int colourId) {
  auto modulator = pool.getModulator(code, number, colourId);
  if (modulator == nullptr) { return nullptr; }
  modulators.push_back(modulator);
  nameToModuleMap[modulator->name] = modulator;
  return modulator;
}

std::vector<std::shared_ptr<Connection>> ModuleManager::getConnectionsOfSource(std::shared_ptr<Module> source) {
  std::vector<std::shared_ptr<Connection>> sourceConnections;

  for (auto connection : connections)
    if (connection->source == source)
      sourceConnections.push_back(connection);

  return sourceConnections;
}

std::vector<std::shared_ptr<Connection>> ModuleManager::getConnectionsOfTarget(std::shared_ptr<Module> target) {
  std::vector<std::shared_ptr<Connection>> targetConnections;

  for (auto connection : connections)
    if (connection->target == target)
      targetConnections.push_back(connection);

  return targetConnections;
}

void ModuleManager::RemoveModulator(int index) {
  auto modulator = modulators[index];
  modulators.erase(modulators.begin() + index);

  for (auto connection : getConnectionsOfSource(modulator))
    removeConnection(connection);

  pool.Retire(modulator);
  nameToModuleMap.erase(modulator->name);
}

void ModuleManager::repositionBlock(Index oldIndex, Index newIndex) {
  auto block = getBlock(oldIndex);

  block->index = newIndex;

  blockMatrix[newIndex.row][newIndex.column] = block;
  blockMatrix[oldIndex.row][oldIndex.column] = {};
}

std::shared_ptr<Connection> ModuleManager::addConnection(std::shared_ptr<Module> source, std::shared_ptr<Module> target, std::string parameter_name, int number) {
  if (connectionExists(parameter_name, source, target)) return nullptr;

  auto connection = pool.getModulation(number);
  // connection->parameterIndex = parameterIndex;
  connection->source = source;
  connection->target = target;
  connections.push_back(connection);
  // target->parameters[parameterIndex]->connections.push_back(connection);
  return connection;
}

bool ModuleManager::connectionExists(std::string parameter_name, std::shared_ptr<Module> source, std::shared_ptr<Module> target) {
  for (auto connection : connections) {
    bool connectionExists = connection->target == target
      && connection->source == source
      && parameter_name == connection->parameter_name_;

    if (connectionExists) return true;
  }
  return false;
}

void ModuleManager::removeConnection(int index) {
  auto connection = connections[index];
  // connection->target->removeConnection(connection);
  connections.erase(connections.begin() + index);
  pool.Retire(connection);
}

void ModuleManager::removeConnection(std::shared_ptr<Connection> connection) {
  // connection->target->removeConnection(connection);
  // pool.retire(connections.removeAndReturn(connections.indexOf(connection)));
}

std::vector<std::shared_ptr<Connection>> ModuleManager::getConnections() {
  std::vector<std::shared_ptr<Connection>> array;
  array.insert(array.end(), connections.begin(), connections.end());
  return array;
}

std::vector<std::shared_ptr<Module>> ModuleManager::getModulators() {
  std::vector<std::shared_ptr<Module>> array;
  array.insert(array.end(), modulators.begin(), modulators.end());
  return array;
}

void ModuleManager::clear() {
  // for (int i = tabs.size() - 1; i >= 0; i--) removeTab(tabs[i]);
  for (int i = blocks.size() - 1; i >= 0; i--) removeBlock(blocks[i]);
  for (int i = connections.size() - 1; i >= 0; i--) removeConnection(i);
  for (int i = modulators.size() - 1; i >= 0; i--) RemoveModulator(i);
}

// void ModuleManager::removeTab(std::shared_ptr<Tab> tab) {
//   for (auto connection : getConnectionsOfTarget(tab)) {
//     removeConnection(connection);
//   }
//   pool.retire(tab);
//   tabs.removeFirstMatchingValue(tab);
//   nameToModuleMap.erase(tab->name);
// }

// void ModuleManager::triggerNoteInTabs(Voice* voice) {
//   voice->setAll(true);
//   for (auto tab : getTabs()) {
//     voice->setColumnsState(tab->getAllColumns(), false);
//     voice->setColumnsState(tab->getNextColumns(static_cast<int>(tab->parameter(static_cast<int>(Tab::Parameters::columns))->getNormalizedValue())), true);
//   }
// }

// Array<int> ModuleManager::getActiveColumns() {
//   Array<int> activeColumns;

//   for (auto tab : tabs)
//     activeColumns.addArray(tab->activeColumns);

//   return activeColumns;
// }

// std::shared_ptr<Tab> ModuleManager::addTab(Model::Type type, int column, int number) {
//   auto tab = pool.getTab(type, number);
//   if (tab == nullptr) return nullptr;
//   tab->column = column;
//   nameToModuleMap[tab->name] = tab;
//   tabs.add(tab);
//   return tab;
// }

// std::shared_ptr<Tab> ModuleManager::getTab(int column) {
//   for (auto tab : tabs) {
//     if (tab->column == column) return tab;
//   }
//   return nullptr;
// }
}