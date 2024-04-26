/*
  ==============================================================================

    ModulePool.h
    Created: 3 Jul 2021 7:35:34pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

// #include "model/ModuleFactory.h"
#include "gui/ColourPool.h"
#include "gui/Tab.h"
#include "ModuleContainer.h"
#include "module_new.h"
#include "connection.h"
#include "column_control_model.h"

// using namespace Model;
namespace model {
struct ModulePool {
public:
  ColourPool colourPool;

  ModuleContainer<model::Block> blocks;
  ModuleContainer<model::Module> modulators;
  std::vector<std::shared_ptr<model::ColumnControl>> column_controls_;
  // ModuleContainer<Tab> tabs;

  std::unordered_map<std::string, std::shared_ptr<model::Module>> name_module_map_;
  // std::unordered_map<model::, std::shared_ptr<model::Module>> name_module_map_;
  std::vector<std::shared_ptr<model::Module>> all_modules_;
  std::vector<std::shared_ptr<model::Connection>> connections;

  ModulePool();
  ~ModulePool();

  // std::shared_ptr<Tab> getTab(std::string code, int number);
  std::shared_ptr<model::Block> acquire_block(std::string code, int number);
  std::shared_ptr<model::Module> acquire_modulator(std::string code, int number, int colourId);
  std::shared_ptr<model::Connection> aquire_connection(int number = -1);
  std::shared_ptr<model::Module> get(std::string name) { return name_module_map_[name]; }

  void retire(std::shared_ptr<model::Connection> modulationConnection);
  void retire(std::shared_ptr<model::Block> block);
  void retire(std::shared_ptr<model::Module> modulator);
  // void retire(std::shared_ptr<Tab> tab);
};
} // namespace model