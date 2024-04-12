/*
  ==============================================================================

    PresetManager.h
    Created: 7 Jul 2021 9:25:24am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "model/PresetInfo.h"
#include "BinaryData.h"

class PresetCoder;
class PresetManager {
private:
  File presetsDirectory;
  PresetCoder* coder;
public:
  std::vector<Preset> presets;
  PresetManager();

  void save(Preset presetData);

  void loadPresetsDirectory();
  void setPresetsDirectory();
  void loadStockPresets();
  void createAndSavePresetFile(std::string& name, const std::string& presetJson) const;
  void removePreset(std::string& name);

  std::string presetToString(Preset preset);
  std::optional<Preset> stringToPreset(std::string preset);
};
