/*
  ==============================================================================

    PresetInfo.h
    Created: 30 May 2021 7:10:45pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "model/Block.h"
#include "gui/Tab.h"
#include "connection.h"

// #include "

class PresetInfo {
public:
  struct Module {
    ID id;
    std::map<String, float> parameters;
  };

  struct Block: public PresetInfo::Module {
    int length = 1;
    std::pair<int, int> index = { -1, -1 };
  };

  struct Modulator: public PresetInfo::Module { int colour; };

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

  String name = "";
  std::vector<Tab> tabs;
  std::vector<Block> blocks;
  std::vector<Modulator> modulators;
  std::vector<Connection> connections_;

  static PresetInfo create(String name,
    // std::vector<std::shared_ptr<model::Tab>> tabs,
    std::vector<std::shared_ptr<model::Block>> blocks,
    std::vector<std::shared_ptr<model::Module>> modulators,
    std::vector<std::shared_ptr<model::Connection>> modulations);
private:
  static void prepareModule(std::shared_ptr<model::Module> module, Module& moduleInfo);
};

