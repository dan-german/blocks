#include "model/module_manager.h"

namespace model {
ModuleManager::~ModuleManager() {
  Clear();
}

std::shared_ptr<model::Block> ModuleManager::AddBlock(std::string code, Index index, int number) {
  auto block = pool.GetBlock(code, number);
  if (block == nullptr) return nullptr;
  block->index = index;
  nameToModuleMap[block->name] = block;
  blockMatrix[index.row][index.column] = block;
  blocks.push_back(block);
  return block;
}

void ModuleManager::RemoveBlock(std::shared_ptr<model::Block> block) {
  for (auto connection : GetConnectionsOfTarget(block)) {
    RemoveConnection(connection);
  }
  nameToModuleMap.erase(block->name);
  blockMatrix[block->index.row][block->index.column] = nullptr;
  // blocks.remove(blocks.indexOf(block));
  blocks.erase(std::remove(blocks.begin(), blocks.end(), block), blocks.end());
  pool.Retire(block);
}

std::shared_ptr<Module> ModuleManager::AddModulator(std::string code, int number, int colourId) {
  auto modulator = pool.GetModulator(code, number, colourId);
  if (modulator == nullptr) { return nullptr; }
  modulators.push_back(modulator);
  nameToModuleMap[modulator->name] = modulator;
  return modulator;
}

std::vector<std::shared_ptr<Modulation>> ModuleManager::GetConnectionsOfSource(std::shared_ptr<Module> source) {
  std::vector<std::shared_ptr<Modulation>> sourceConnections;

  for (auto connection : connections)
    if (connection->source == source)
      sourceConnections.push_back(connection);

  return sourceConnections;
}

std::vector<std::shared_ptr<Modulation>> ModuleManager::GetConnectionsOfTarget(std::shared_ptr<Module> target) {
  std::vector<std::shared_ptr<Modulation>> targetConnections;

  for (auto connection : connections)
    if (connection->target == target)
      targetConnections.push_back(connection);

  return targetConnections;
}

void ModuleManager::RemoveModulator(int index) {
  auto modulator = modulators[index];
  modulators.erase(modulators.begin() + index);

  for (auto connection : GetConnectionsOfSource(modulator))
    RemoveConnection(connection);

  pool.Retire(modulator);
  nameToModuleMap.erase(modulator->name);
}

void ModuleManager::RepositionBlock(Index oldIndex, Index newIndex) {
  auto block = GetBlock(oldIndex);

  block->index = newIndex;

  blockMatrix[newIndex.row][newIndex.column] = block;
  blockMatrix[oldIndex.row][oldIndex.column] = {};
}

std::shared_ptr<Modulation> ModuleManager::AddConnection(std::shared_ptr<Module> source, std::shared_ptr<Module> target, int parameterIndex, int number) {
  if (ConnectionExists(parameterIndex, source, target)) return nullptr;

  auto connection = pool.GetModulation(number);
  connection->parameterIndex = parameterIndex;
  connection->source = source;
  connection->target = target;
  connections.push_back(connection);
  // target->parameters[parameterIndex]->connections.push_back(connection);
  return connection;
}

bool ModuleManager::ConnectionExists(int parameterIndex, std::shared_ptr<Module> source, std::shared_ptr<Module> target) {
  for (auto connection : connections) {
    bool connectionExists = connection->target == target
      && connection->source == source
      && parameterIndex == connection->parameterIndex;

    if (connectionExists) return true;
  }
  return false;
}

void ModuleManager::RemoveConnection(int index) {
  auto connection = connections[index];
  // connection->target->removeConnection(connection);
  connections.erase(connections.begin() + index);
  pool.Retire(connection);
}

void ModuleManager::RemoveConnection(std::shared_ptr<Modulation> connection) {
  // connection->target->removeConnection(connection);
  // pool.retire(connections.removeAndReturn(connections.indexOf(connection)));
}

std::vector<std::shared_ptr<Modulation>> ModuleManager::GetConnections() {
  std::vector<std::shared_ptr<Modulation>> array;
  array.insert(array.end(), connections.begin(), connections.end());
  return array;
}

std::vector<std::shared_ptr<Module>> ModuleManager::GetModulators() {
  std::vector<std::shared_ptr<Module>> array;
  array.insert(array.end(), modulators.begin(), modulators.end());
  return array;
}

void ModuleManager::Clear() {
  // for (int i = tabs.size() - 1; i >= 0; i--) removeTab(tabs[i]);
  for (int i = blocks.size() - 1; i >= 0; i--) RemoveBlock(blocks[i]);
  for (int i = connections.size() - 1; i >= 0; i--) RemoveConnection(i);
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