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
#include "lfo_model.h"
#include "adsr_module_new.h"
#include "reverb_module_new.h"
#include "delay_module_new.h"
#include "drive_module.h"
#include "chorus_model.h"
#include "flanger_model.h"  
#include "phaser_model.h"  
#include "column_control_model.h"
#include "noise_model.h"
#include "rand_model.h"

namespace model {
// Module
ModulePool::~ModulePool() { }

// std::shared_ptr<Tab> ModulePool::getTab(Type type, int number) { return tabs.get({ type, number }); }
std::shared_ptr<Block> ModulePool::dequeueBlock(std::string type, int number = -1) { return blocks_.get({ type, number }); }
std::shared_ptr<Connection> ModulePool::dequeueConnection(int number) { return connections_.get({ "modulation", number }); }

void ModulePool::retire(std::shared_ptr<model::Connection> modulationConnection) { connections_.retire(modulationConnection); }
void ModulePool::retire(std::shared_ptr<Block> block) { blocks_.retire(block); }
// void ModulePool::Retire(std::shared_ptr<Tab> tab) { tabs.retire(tab); }

void ModulePool::retire(std::shared_ptr<Module> modulator) {
  colourPool.retire(modulator->colour.id);
  modulators_.retire(modulator);
}

ModulePool::ModulePool() {
  blocks_.spawn({ "osc" }, [](std::string type, int number) { return std::make_shared<model::OscillatorModule>(number); });
  blocks_.spawn({ "filter" }, [](std::string type, int number) { return std::make_shared<model::FilterModule>(number); });
  blocks_.spawn({ "reverb" }, [](std::string type, int number) { return std::make_shared<model::ReverbModule>(number); });
  blocks_.spawn({ "delay" }, [](std::string type, int number) { return std::make_shared<model::DelayModule>(number); });
  blocks_.spawn({ "drive" }, [](std::string type, int number) { return std::make_shared<model::DriveModule>(number); });
  blocks_.spawn({ "chorus" }, [](std::string type, int number) { return std::make_shared<model::ChorusModule>(number); });
  blocks_.spawn({ "flanger" }, [](std::string type, int number) { return std::make_shared<model::FlangerModule>(number); });
  blocks_.spawn({ "phaser" }, [](std::string type, int number) { return std::make_shared<model::PhaserModule>(number); });
  blocks_.spawn({ "noise" }, [](std::string type, int number) { return std::make_shared<model::NoiseModel>(number); });

  modulators_.spawn({ "lfo" }, [](std::string type, int number) { return std::make_shared<model::LFOModule>(number); });
  modulators_.spawn({ "envelope" }, [](std::string type, int number) { return std::make_shared<model::ADSRModule>(number); });
  modulators_.spawn({ "random" }, [](std::string type, int number) { return std::make_shared<model::RandModel>(number); });
  connections_.spawn({ "modulation" }, [](std::string type, int number) { return std::make_shared<model::Connection>(number); }, 40);

  // for (int i = 1; i <= 40; i++) connections.push_back(std::make_shared<Connection>(i));
  for (int i = 1; i <= Constants::columns; i++) column_controls_.push_back(std::make_shared<ColumnControl>(i));

  all_modules_.insert(all_modules_.end(), blocks_.all.begin(), blocks_.all.end());
  all_modules_.insert(all_modules_.end(), modulators_.all.begin(), modulators_.all.end());
  all_modules_.insert(all_modules_.end(), column_controls_.begin(), column_controls_.end());
  all_modules_.insert(all_modules_.end(), connections_.all.begin(), connections_.all.end());
  for (auto module : all_modules_) name_module_map_[module->name] = module;
}


std::shared_ptr<model::Module> ModulePool::dequeueModulator(std::string code, int number, int colourId) {
  auto modulator = modulators_.get({ code, number });
  if (modulator) modulator->colour = colourPool.get(colourId);
  return modulator;
}

// std::shared_ptr<Connection> ModulePool::dequeueConnection(int number) {
//   int index = 0;
//   for (int i = 0; i < connections.size(); i++) {
//     if (connections[i]->id.number == number) {
//       index = 0;
//     }
//   }
//   auto connection = connections[index];
//   connections.erase(connections.begin());
//   return connection;
// }
} // namespace model