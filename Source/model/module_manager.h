#include <memory>

#include "module_pool.h"
// #include "dsp/Voice.h"
#include "gui/ColourPool.h"
#include "settings/Constants.h"
#include "module_new.h"
#include "modulation_new.h"
// #include "model/PresetManager.h"

// using Module = Model::Module;
// using Block = Model::Block;
// using Modulation = Model::Modulation;

namespace model {
class ModuleManager {
private:
  std::shared_ptr<model::Block> blockMatrix[Constants::rows][Constants::columns];
  std::map<String, std::shared_ptr<model::Module>> nameToModuleMap;
  std::vector<std::shared_ptr<model::Modulation>> connections;
  std::vector<std::shared_ptr<model::Block>> blocks;
  ColourPool colourPool;
public:
  ModulePool pool;
  std::vector<std::shared_ptr<model::Module>> modulators;
  // Array<std::shared_ptr<model::Tab>> tabs;

  ~ModuleManager();

  // std::shared_ptr<Tab> addTab(Model::Type type, int column, int number = 1);
  // std::shared_ptr<Tab> getTab(int column);
  // Array<std::shared_ptr<Tab>> getTabs() { return tabs; }
  // void removeTab(std::shared_ptr<Tab> tab);
  // void triggerNoteInTabs(Voice* voice);
  // Array<int> getActiveColumns();

  std::shared_ptr<model::Block> AddBlock(std::string type, Index index, int number = -1);
  std::shared_ptr<model::Block> GetBlock(Index index) { return blockMatrix[index.row][index.column]; }
  std::vector<std::shared_ptr<model::Block>> GetBlocks() { return blocks; }
  void RemoveBlock(std::shared_ptr<model::Block> block);
  void RepositionBlock(Index oldIndex, Index newIndex);

  std::shared_ptr<model::Module> AddModulator(std::string type, int number, int colourId);
  std::shared_ptr<model::Module> GetModulator(int index) { return modulators[index]; }
  std::vector<std::shared_ptr<model::Module>> GetModulators();
  void RemoveModulator(int index);

  std::shared_ptr<model::Modulation> AddConnection(std::shared_ptr<Module> source, std::shared_ptr<Module> target, int parameterIndex, int number = -1);
  std::shared_ptr<model::Modulation> GetConnection(int index) { return connections[index]; }
  std::vector<std::shared_ptr<model::Modulation>> GetConnectionsOfSource(std::shared_ptr<Module> source);
  std::vector<std::shared_ptr<model::Modulation>> GetConnectionsOfTarget(std::shared_ptr<Module> target);
  std::vector<std::shared_ptr<model::Modulation>> GetConnections();
  void RemoveConnection(int index);
  void RemoveConnection(std::shared_ptr<model::Modulation> connection);
  bool ConnectionExists(int parameterIndex, std::shared_ptr<model::Module> source, std::shared_ptr<model::Module> target);
  std::shared_ptr<model::Module> GetModule(String name) { return nameToModuleMap[name]; }
  void Clear();
};
} // namespace model