#include <memory>

#include "ModulePool.h"
#include "Voice.h"
#include "ColourPool.h"
#include "Constants.h"
#include "PresetManager.h"

using Module = Model::Module;
using Block = Model::Block;
using Modulation = Model::Modulation;

class ModuleManager {
private:
  std::shared_ptr<Block> blockMatrix[Constants::rows][Constants::columns];
  std::map<String, std::shared_ptr<Module>> nameToModuleMap;
  Array<std::shared_ptr<Modulation>> connections;
  Array<std::shared_ptr<Block>> blocks;
  ColourPool colourPool;
public:
  ModulePool pool;
  Array<std::shared_ptr<Module>> modulators;
  Array<std::shared_ptr<Tab>> tabs;

  ~ModuleManager();

  std::shared_ptr<Tab> addTab(Model::Type type, int column, int number = 1);
  std::shared_ptr<Tab> getTab(int column);
  Array<std::shared_ptr<Tab>> getTabs() { return tabs; }
  void removeTab(std::shared_ptr<Tab> tab);
  void triggerNoteInTabs(Voice* voice);
  Array<int> getActiveColumns();

  std::shared_ptr<Block> addBlock(Model::Type type, Index index, int number = -1);
  std::shared_ptr<Block> getBlock(Index index) { return blockMatrix[index.row][index.column]; }
  Array<std::shared_ptr<Block>> getBlocks() { return blocks; }
  void removeBlock(std::shared_ptr<Block> block);
  void repositionBlock(Index oldIndex, Index newIndex);

  std::shared_ptr<Module> addModulator(Model::Type code, int number, int colourId);
  std::shared_ptr<Module> getModulator(int index) { return modulators.getUnchecked(index); }
  Array<std::shared_ptr<Module>> getModulators();
  void removeModulator(int index);

  std::shared_ptr<Modulation> addConnection(std::shared_ptr<Module> source, std::shared_ptr<Module> target, int parameterIndex, int number = -1);
  std::shared_ptr<Modulation> getConnection(int index) { return connections.getUnchecked(index); }
  Array<std::shared_ptr<Modulation>> getConnectionsOfSource(std::shared_ptr<Module> source);
  Array<std::shared_ptr<Modulation>> getConnectionsOfTarget(std::shared_ptr<Module> target);
  Array<std::shared_ptr<Modulation>> getConnections();
  void removeConnection(int index);
  void removeConnection(std::shared_ptr<Modulation> connection);
  bool connectionExists(int parameterIndex, std::shared_ptr<Module> source, std::shared_ptr<Module> target);
  std::shared_ptr<Module> getModule(String name) { return nameToModuleMap[name]; }
  void clear();
};
