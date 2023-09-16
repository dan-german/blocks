/*
  ==============================================================================

    PresetCoder.h
    Created: 30 May 2021 6:44:48pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "json.hpp"
#include "PresetInfo.h"
#include "ModuleParameter.h"
#include "Modulation.h"
#include "Block.h"
#include <vector>
#include "PresetManager.h"

using json = nlohmann::json;
using Module = Model::Module;
using Block = Model::Block;
using ModuleParameter = Model::ModuleParameter;
using Modulation = Model::Modulation;

class PresetManager;

class PresetCoder {
public:
  std::string encode(PresetInfo presetData);
  std::optional<PresetInfo> decode(std::string jsonString);
private:
  void decodeModule(json& moduleJson, PresetInfo::Module& module);
  json encodeModule(PresetInfo::Module module);
  json encodeBlocks(Array<PresetInfo::Block> blocks);
  json encodeModulations(Array<PresetInfo::Modulation> modulationConnections);
  json encodeModulators(Array<PresetInfo::Modulator> modulators);
  json encodeTabs(Array<PresetInfo::Tab> modules);
};