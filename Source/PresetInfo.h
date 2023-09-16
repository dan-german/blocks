/*
  ==============================================================================

    PresetInfo.h
    Created: 30 May 2021 7:10:45pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "Block.h"
#include "Tab.h"

using namespace std;

class PresetInfo {
public:
  struct Module {
    Model::ID id;
    std::map<String, float> parameters;
  };

  struct Block: public PresetInfo::Module {
    int length = 1;
    pair<int, int> index = { -1, -1 };
  };

  struct Modulator: public PresetInfo::Module { int colour; };

  struct Modulation {
    string source;
    string target;
    string parameter;
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
    Array<shared_ptr<Model::Tab>> tabs,
    Array<shared_ptr<Model::Block>> blocks,
    Array<shared_ptr<Model::Module>> modulators,
    Array<shared_ptr<Model::Modulation>> modulations);
private:
  static void prepareModule(shared_ptr<Model::Module> module, Module& moduleInfo);
};

