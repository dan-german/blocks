/*
  ==============================================================================

    Block.h
    Created: 20 Dec 2021 1:17:40pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "model/Module.h"

namespace Model {
struct Block: public Module {
  Block(Type type, int number): Module(type, number) { };
  Index index = { -1, -1 };
  void reset() override {
    Module::reset();
    index = { -1, -1 };
  };
};
}