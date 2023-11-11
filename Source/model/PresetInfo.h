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

  struct Modulation {
    std::string source;
    std::string target;
    std::string parameter;
    float magnitude;
    bool bipolar;
    int number;
  };

  struct Tab: Module {
    int column;
    int length;
  };

  String name = "";
  Array<Tab> tabs;
  Array<Block> blocks;
  Array<Modulator> modulators;
  Array<Modulation> modulations;

  static PresetInfo create(String name,
    Array<std::shared_ptr<Model::Tab>> tabs,
    Array<std::shared_ptr<Model::Block>> blocks,
    Array<std::shared_ptr<Model::Module>> modulators,
    Array<std::shared_ptr<Model::Modulation>> modulations);
private:
  static void prepareModule(std::shared_ptr<Model::Module> module, Module& moduleInfo);
};

