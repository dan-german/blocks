/*
  ==============================================================================

    ModulePool.cpp
    Created: 3 Jul 2021 7:35:34pm
    Author:  Dan German

  ==============================================================================
*/

#include "module_pool.h"
#include "oscillator_module_new.h"
#include "filter_module_new.h"
#include "lfo_module_new.h"
#include "adsr_module_new.h"
#include "reverb_module_new.h"
#include "delay_module_new.h"
#include "drive_module.h"
#include "chorus_model.h"
#include "flanger_model.h"  
#include "phaser_model.h"  
#include "column_control_model.h"
#include "noise_model.h"

namespace model {
// Module
ModulePool::~ModulePool() { }

// std::shared_ptr<Tab> ModulePool::getTab(Type type, int number) { return tabs.get({ type, number }); }
std::shared_ptr<Block> ModulePool::getBlock(std::string type, int number = -1) { return blocks.get({ type, number }); }
void ModulePool::retire(std::shared_ptr<Block> block) { blocks.retire(block); }
// void ModulePool::Retire(std::shared_ptr<Tab> tab) { tabs.retire(tab); }

void ModulePool::retire(std::shared_ptr<Module> modulator) {
  colourPool.retire(modulator->colour.id);
  modulators.retire(modulator);
}

ModulePool::ModulePool() {
  blocks.spawn({ "osc" }, [](std::string type, int number) { return std::make_shared<model::OscillatorModule>(number); });
  blocks.spawn({ "filter" }, [](std::string type, int number) { return std::make_shared<model::FilterModule>(number); });
  blocks.spawn({ "reverb" }, [](std::string type, int number) { return std::make_shared<model::ReverbModule>(number); });
  blocks.spawn({ "delay" }, [](std::string type, int number) { return std::make_shared<model::DelayModule>(number); });
  blocks.spawn({ "drive" }, [](std::string type, int number) { return std::make_shared<model::DriveModule>(number); });
  blocks.spawn({ "chorus" }, [](std::string type, int number) { return std::make_shared<model::ChorusModule>(number); });
  blocks.spawn({ "flanger" }, [](std::string type, int number) { return std::make_shared<model::FlangerModule>(number); });
  blocks.spawn({ "phaser" }, [](std::string type, int number) { return std::make_shared<model::PhaserModule>(number); });
  blocks.spawn({ "noise" }, [](std::string type, int number) { return std::make_shared<model::NoiseModel>(number); });

  modulators.spawn({ "lfo" }, [](std::string type, int number) { return std::make_shared<model::LFOModule>(number); });
  modulators.spawn({ "envelope" }, [](std::string type, int number) { return std::make_shared<model::ADSRModule>(number); });

  for (int i = 1; i <= 40; i++) connections.push_back(std::make_shared<Connection>(i));
  for (int i = 1; i <= Constants::columns; i++) column_controls_.push_back(std::make_shared<ColumnControl>(i));

  all_modules_.insert(all_modules_.end(), blocks.all.begin(), blocks.all.end());
  all_modules_.insert(all_modules_.end(), modulators.all.begin(), modulators.all.end());
  all_modules_.insert(all_modules_.end(), column_controls_.begin(), column_controls_.end());
}

void ModulePool::retire(std::shared_ptr<model::Connection> modulationConnection) {
  modulationConnection->reset();
  connections.push_back(modulationConnection);
}

std::shared_ptr<model::Module> ModulePool::getModulator(std::string code, int number, int colourId) {
  auto modulator = modulators.get({ code, number });
  if (modulator) modulator->colour = colourPool.get(colourId);
  return modulator;
}

std::shared_ptr<Connection> ModulePool::getConnection(int number) {
  int index = 0;
  for (int i = 0; i < connections.size(); i++) {
    if (connections[i]->number == number) {
      index = 0;
    }
  }
  auto connection = connections[index];
  connections.erase(connections.begin());
  return connection;
}
} // namespace model