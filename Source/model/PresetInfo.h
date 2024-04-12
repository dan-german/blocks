/*
  ==============================================================================

    PresetInfo.h
    Created: 30 May 2021 7:10:45pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
// #include "model/Block.h"
#include "gui/Tab.h"
#include "connection.h"
#include "model/column_control_model.h"

class Preset {
public:
  struct Module {
    ID id;
    std::map<std::string, float> parameters;
  };

  struct Block: public Preset::Module {
    int length = 1;
    std::pair<int, int> index = { -1, -1 };
  };

  struct Modulator: public Preset::Module { int colour; };

  struct Connection {
    std::string source;
    std::string target;
    std::string parameter;
    float amount;
    bool bipolar;
    int number;
  };

  struct Tab: Module {
    int column;
    int length;
  };

  std::string name = "";
  std::vector<Tab> tabs;
  std::vector<Block> blocks;
  std::vector<Modulator> modulators;
  std::vector<Connection> connections_;
  std::vector<Module> column_controls;

  static Preset create(std::string name,
    // std::vector<std::shared_ptr<model::Tab>> tabs,
    std::vector<std::shared_ptr<model::Block>> blocks,
    std::vector<std::shared_ptr<model::Module>> modulators,
    std::vector<std::shared_ptr<model::Connection>> modulations, 
    std::vector<std::shared_ptr<model::ColumnControl>> column_controls);
private:
  static void setParamsAndID(std::shared_ptr<model::Module> module, Module& moduleInfo);
};

