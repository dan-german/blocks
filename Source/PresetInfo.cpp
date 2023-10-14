/*
  ==============================================================================

    PresetInfo.cpp
    Created: 30 May 2021 7:10:45pm
    Author:  Dan German

  ==============================================================================
*/

#include "PresetInfo.h"
#include "PresetManager.h"
#include "ModuleParameter.h"
#include "Tab.h"

PresetInfo PresetInfo::create(String name,
  Array<std::shared_ptr<Model::Tab>> tabs,
  Array<std::shared_ptr<Model::Block>> blocks,
  Array<std::shared_ptr<Model::Module>> modulators,
  Array<std::shared_ptr<Model::Modulation>> modulations) {
  PresetInfo info;

  info.name = name;

  for (auto tab : tabs) {
    Tab tabInfo;
    prepareModule(tab, tabInfo);
    tabInfo.column = tab->column;
    tabInfo.length = tab->length;
    info.tabs.add(tabInfo);
  }

  for (auto block : blocks) {
    if (block->isChild) continue;
    Block blockInfo;
    PresetInfo::prepareModule(block, blockInfo);
    blockInfo.index = { block->index.row, block->index.column };
    blockInfo.length = block->length;

    info.blocks.add(blockInfo);
  }

  for (auto modulator : modulators) {
    Modulator modulatorInfo;
    PresetInfo::prepareModule(modulator, modulatorInfo);
    modulatorInfo.colour = modulator->colour.id;
    info.modulators.add(modulatorInfo);
  }

  for (auto modulation : modulations) {
    Modulation modulationInfo;

    modulationInfo.parameter = modulation->target->parameter(modulation->parameterIndex)->id.toStdString();
    modulationInfo.source = modulation->source->name.toStdString();
    modulationInfo.target = modulation->target->name.toStdString();
    modulationInfo.number = modulation->number;
    modulationInfo.bipolar = static_cast<bool>(modulation->bipolarParameter->getValue());
    modulationInfo.magnitude = modulation->magnitudeParameter->getValue();

    info.modulations.add(modulationInfo);
  }

  return info;
}

void PresetInfo::prepareModule(std::shared_ptr<Model::Module> module, Module& moduleInfo) {
  moduleInfo.id = {
    .type = module->id.type,
    .number = module->id.number
  };

  for (auto parameter : module->parameters)
    moduleInfo.parameters[parameter->id] = parameter->audioParameter->getValue();
}