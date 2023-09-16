#include <memory>

#include "ModulePool.h"
#include "Voice.h"
#include "ColourPool.h"
#include "Constants.h"
#include "PresetManager.h"

using Module = Model::Module;
using Block = Model::Block;
using Modulation = Model::Modulation;

using namespace std;

class ModuleManager {
private:
  template<typename Key, typename Value>
  using map = std::map<Key, Value>;
  shared_ptr<Block> blockMatrix[Constants::rows][Constants::columns];
  map<String, shared_ptr<Module>> nameToModuleMap;
  Array<shared_ptr<Modulation>> connections;
  Array<shared_ptr<Block>> blocks;
  ColourPool colourPool;
public:
  ModulePool pool;
  Array<shared_ptr<Module>> modulators;
  Array<shared_ptr<Tab>> tabs;

  ~ModuleManager();

  shared_ptr<Tab> addTab(Model::Type type, int column, int number = 1);
  shared_ptr<Tab> getTab(int column);
  Array<shared_ptr<Tab>> getTabs() { return tabs; }
  void removeTab(shared_ptr<Tab> tab);
  void triggerNoteInTabs(Voice* voice);
  Array<int> getActiveColumns();

  shared_ptr<Block> addBlock(Model::Type type, Index index, int number = -1);
  shared_ptr<Block> getBlock(Index index) { return blockMatrix[index.row][index.column]; }
  Array<shared_ptr<Block>> getBlocks() { return blocks; }
  void removeBlock(shared_ptr<Block> block);
  void repositionBlock(Index oldIndex, Index newIndex);

  shared_ptr<Module> addModulator(Model::Type code, int number, int colourId);
  shared_ptr<Module> getModulator(int index) { return modulators.getUnchecked(index); }
  Array<shared_ptr<Module>> getModulators();
  void removeModulator(int index);

  shared_ptr<Modulation> addConnection(shared_ptr<Module> source, shared_ptr<Module> target, int parameterIndex, int number = -1);
  shared_ptr<Modulation> getConnection(int index) { return connections.getUnchecked(index); }
  Array<shared_ptr<Modulation>> getConnectionsOfSource(shared_ptr<Module> source);
  Array<shared_ptr<Modulation>> getConnectionsOfTarget(shared_ptr<Module> target);
  Array<shared_ptr<Modulation>> getConnections();
  void removeConnection(int index);
  void removeConnection(shared_ptr<Modulation> connection);
  bool connectionExists(int parameterIndex, shared_ptr<Module> source, shared_ptr<Module> target);
  shared_ptr<Module> getModule(String name) { return nameToModuleMap[name]; }
  void clear();
};
