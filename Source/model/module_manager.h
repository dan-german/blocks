#pragma once
#include <memory>

#include "module_pool.h"
#include "gui/ColourPool.h"
#include "settings/Constants.h"
#include "module_new.h"
#include "connection.h"
#include "model/column_control_model.h"

namespace model {
class ModuleManager {
private:
  std::shared_ptr<model::Block> blockMatrix[Constants::columns][Constants::rows];
  std::map<std::string, std::shared_ptr<model::Module>> nameToModuleMap;
  std::vector<std::shared_ptr<model::Connection>> connections;
  std::vector<std::shared_ptr<model::Block>> blocks;
  ColourPool colourPool;
public:
  ModulePool pool;
  std::vector<std::shared_ptr<model::Module>> modulators;
  // Array<std::shared_ptr<model::Tab>> tabs;
  std::vector<std::shared_ptr<model::ColumnControl>> column_controls_;

  ModuleManager();
  ~ModuleManager();

  // std::shared_ptr<Tab> addTab(Model::Type type, int column, int number = 1);
  // std::shared_ptr<Tab> getTab(int column);
  // Array<std::shared_ptr<Tab>> getTabs() { return tabs; }
  // void removeTab(std::shared_ptr<Tab> tab);
  // void triggerNoteInTabs(Voice* voice);
  // Array<int> getActiveColumns();

  std::shared_ptr<model::ColumnControl> getColumnControl(int index) { return column_controls_[index]; }

  std::shared_ptr<model::Block> addBlock(std::string type, Index index, int number = -1);
  std::shared_ptr<model::Block> getBlock(Index index) { return blockMatrix[index.column][index.row]; }
  std::vector<std::shared_ptr<model::Block>> getBlocks() { return blocks; }
  void removeBlock(std::shared_ptr<model::Block> block);
  void repositionBlock(Index oldIndex, Index newIndex);

  std::shared_ptr<model::Module> addModulator(std::string type, int number, int colourId);
  std::shared_ptr<model::Module> getModulator(int index) { return modulators[index]; }
  std::vector<std::shared_ptr<model::Module>> getModulators();
  void removeModulator(int index);

  std::shared_ptr<model::Connection> addConnection(std::shared_ptr<Module> source, std::shared_ptr<Module> target, std::string parameter_name, int number = -1);
  std::shared_ptr<model::Connection> getConnection(int index) { return connections[index]; }
  std::vector<std::shared_ptr<model::Connection>> getConnectionsOfSource(std::shared_ptr<Module> source);
  std::vector<std::shared_ptr<model::Connection>> getConnectionsOfTarget(std::shared_ptr<Module> target);
  std::vector<std::shared_ptr<model::Connection>> getConnections();
  void removeConnection(int index);
  void removeConnection(std::shared_ptr<model::Connection> connection);
  bool connectionExists(std::string parameter_name, std::shared_ptr<model::Module> source, std::shared_ptr<model::Module> target);
  std::shared_ptr<model::Module> getModule(std::string name) { return nameToModuleMap[name]; }
  void clear();
};
} // namespace model