/*
  ==============================================================================

    PresetInfo.cpp
    Created: 30 May 2021 7:10:45pm
    Author:  Dan German

  ==============================================================================
*/

#include "model/PresetInfo.h"
#include "model/PresetManager.h"
#include "model/ModuleParameter.h"
#include "module_new.h"
#include "gui/Tab.h"

Preset Preset::create(String name,
  // std::vector<std::shared_ptr<model::Tab>> tabs,
  std::vector<std::shared_ptr<model::Block>> blocks,
  std::vector<std::shared_ptr<model::Module>> modulators,
  std::vector<std::shared_ptr<model::Connection>> connections,
  std::vector<std::shared_ptr<model::ColumnControl>> column_controls) {
  Preset info;

  info.name = name;

  // for (auto tab : tabs) {
  //   Tab tabInfo;
  //   prepareModule(tab, tabInfo);
  //   tabInfo.column = tab->column;
  //   tabInfo.length = tab->length;
  //   info.tabs.add(tabInfo);
  // }

  for (auto block : blocks) {
    if (block->isChild) continue;
    Block blockInfo;
    Preset::setParamsAndID(block, blockInfo);
    blockInfo.index = { block->index.column, block->index.row };
    blockInfo.length = block->length;

    info.blocks.push_back(blockInfo);
  }

  for (auto modulator : modulators) {
    Modulator modulatorInfo;
    Preset::setParamsAndID(modulator, modulatorInfo);
    modulatorInfo.colour = modulator->colour.id;
    info.modulators.push_back(modulatorInfo);
  }

  for (auto connection : connections) {
    Connection modulationInfo;

    modulationInfo.parameter = connection->parameter_name_;
    modulationInfo.source = connection->source->name;
    modulationInfo.target = connection->target->name;
    modulationInfo.number = connection->number;
    modulationInfo.bipolar = static_cast<bool>(connection->bipolar_parameter_->value_processor->value());
    modulationInfo.amount = connection->amount_parameter_->value_processor->value();

    info.connections_.push_back(modulationInfo);
  }


  for (auto column : column_controls) {
    Module columnInfo;
    setParamsAndID(column, columnInfo);
    info.column_controls.push_back(columnInfo);
  }

  return info;
}

void Preset::setParamsAndID(std::shared_ptr<model::Module> module, Module& moduleInfo) {
  moduleInfo.id = {
    .type = module->id.type,
    .number = module->id.number
  };

  for (auto parameter : module->parameters_)
    moduleInfo.parameters[parameter->name] = parameter->value_processor->value();
}