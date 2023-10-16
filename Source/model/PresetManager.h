/*
  ==============================================================================

    PresetManager.h
    Created: 7 Jul 2021 9:25:24am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "model/PresetInfo.h"
#include "model/Module.h"
#include "model/Block.h"
#include "model/Modulation.h"
#include "BinaryData.h"

using Module = Model::Module;
using Block = Model::Block;
using Modulation = Model::Modulation;

class PresetCoder;
class PresetManager {
private:
  File presetsDirectory;
  PresetCoder* coder;
public:
  Array<PresetInfo> presets;
  PresetManager();

  void save(PresetInfo presetData);

  void loadPresetsDirectory();
  void setPresetsDirectory();
  void loadStockPresets();
  void createAndSavePresetFile(String& name, const std::string& presetJson) const;
  void removePreset(String& name);

  std::string presetToString(PresetInfo preset);
  std::optional<PresetInfo> stringToPreset(std::string preset);
};
