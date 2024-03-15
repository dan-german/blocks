/*
  ==============================================================================

    PresetManager.cpp
    Created: 7 Jul 2021 9:25:24am
    Author:  Dan German

  ==============================================================================
*/

#include "model/PresetManager.h"
#include "PresetCoder.h"

std::string PresetManager::presetToString(Preset presetData) { return coder->encode(presetData); }
std::optional<Preset> PresetManager::stringToPreset(std::string preset) { return coder->decode(preset); }

PresetManager::PresetManager(): coder(new PresetCoder()) {
  setPresetsDirectory();
  loadPresetsDirectory();
}

void PresetManager::setPresetsDirectory() {
  File homeDirectory = File::getSpecialLocation(File::userHomeDirectory);
  presetsDirectory = homeDirectory.getChildFile("Music").getChildFile("blocks").getChildFile("Presets");
}

void PresetManager::loadPresetsDirectory() {
  auto files = presetsDirectory.findChildFiles(2, false);

  for (auto file : files) {
    if (file.getFileExtension() != ".blocks") continue;

    auto stringFile = file.loadFileAsString().toStdString();
    if (stringFile == "") continue;

    try {
      if (auto preset = coder->decode(stringFile))
        presets.push_back(*preset);
    } catch (const std::exception& e) {
      DBG("Error decoding " + file.getFullPathName());
    }
  }
}

void PresetManager::loadStockPresets() {
  auto count = BinaryData::namedResourceListSize;

  for (int i = 0; i < count; i++) {
    String fileName = BinaryData::namedResourceList[i];

    if (fileName.contains(StringRef("blocks"))) {
      int size;
      auto preset = BinaryData::getNamedResource(BinaryData::namedResourceList[i], size);
      std::string presetString;

      for (int j = 0; j < size; j++)
        presetString += preset[j];
    }
  }
}

void PresetManager::save(Preset presetData) {
  removePreset(presetData.name);

  auto jsonPreset = coder->encode(presetData);
  // std::cout << jsonPreset << std::endl;
  createAndSavePresetFile(presetData.name, jsonPreset);

  if (auto preset = coder->decode(jsonPreset))
    presets.push_back(*preset);
}

void PresetManager::removePreset(std::string& name) {
  for (int i = 0; i < presets.size(); i++) {
    if (presets[i].name == name) {
      presetsDirectory.getChildFile(name).withFileExtension("blocks").deleteFile();
      // presets.remove(i);
      // remove the preset from the vector
      presets.erase(presets.begin() + i);
      break;
    }
  }
}

void PresetManager::createAndSavePresetFile(std::string& name, const std::string& presetJson) const {
  auto newFile = presetsDirectory.getChildFile(name).withFileExtension("blocks");
  newFile.create();
  newFile.appendText(presetJson);
}