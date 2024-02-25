/*
  ==============================================================================

    PresetCoder.h
    Created: 30 May 2021 6:44:48pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "util/json.hpp"
#include "model/PresetInfo.h"
#include "model/ModuleParameter.h"
#include "model/Modulation.h"
#include "model/Block.h"
#include <vector>
#include "model/PresetManager.h"

using json = nlohmann::json;
using Module = Model::Module;
using Block = Model::Block;
using ModuleParameter = Model::ModuleParameter;
using Modulation = Model::Modulation;

class PresetManager;

class PresetCoder {
public:
  std::string encode(Preset presetData);
  std::optional<Preset> decode(std::string jsonString);
private:
  void decodeModule(json& moduleJson, Preset::Module& module);
  json encodeModule(Preset::Module module);
  json encodeBlocks(std::vector<Preset::Block> blocks);
  json encodeModulations(std::vector<Preset::Connection> modulationConnections);
  json encodeModulators(std::vector<Preset::Modulator> modulators);
  json encodeTabs(std::vector<Preset::Tab> modules);
};