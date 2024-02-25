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

PresetInfo PresetInfo::create(String name,
  // std::vector<std::shared_ptr<model::Tab>> tabs,
  std::vector<std::shared_ptr<model::Block>> blocks,
  std::vector<std::shared_ptr<model::Module>> modulators,
  std::vector<std::shared_ptr<model::Connection>> connections) {
  PresetInfo info;

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
    PresetInfo::prepareModule(block, blockInfo);
    blockInfo.index = { block->index.row, block->index.column };
    blockInfo.length = block->length;

    info.blocks.push_back(blockInfo);
  }

  for (auto modulator : modulators) {
    Modulator modulatorInfo;
    PresetInfo::prepareModule(modulator, modulatorInfo);
    modulatorInfo.colour = modulator->colour.id;
    info.modulators.push_back(modulatorInfo);
  }

  for (auto connection : connections) {
    Connection modulationInfo;

    // modulationInfo.parameter = modulation->target->parameter(modulation->parameterIndex)->id.toStdString();
    modulationInfo.parameter = connection->target->parameter_map_[connection->parameter_name_]->val->value();
    modulationInfo.source = connection->source->name;
    modulationInfo.target = connection->target->name;
    modulationInfo.number = connection->number;
    modulationInfo.bipolar = static_cast<bool>(connection->bipolar_parameter_->val->value());
    modulationInfo.amount = connection->amount_parameter_->val->value();

    info.connections_.push_back(modulationInfo);
  }

  return info;
}

void PresetInfo::prepareModule(std::shared_ptr<model::Module> module, Module& moduleInfo) {
  moduleInfo.id = {
    .type = module->id.type,
    .number = module->id.number
  };

  for (auto parameter : module->parameters_)
    moduleInfo.parameters[parameter->name] = parameter->val->value();
}