/*
  ==============================================================================

    PresetManager.cpp
    Created: 7 Jul 2021 9:25:24am
    Author:  Dan German

  ==============================================================================
*/

#include "PresetManager.h"
#include "PresetCoder.h"

std::string PresetManager::presetToString(PresetInfo presetData) { return coder->encode(presetData); }
std::optional<PresetInfo> PresetManager::stringToPreset(std::string preset) { return coder->decode(preset); }

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
        presets.add(*preset);
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

void PresetManager::save(PresetInfo presetData) {
  removePreset(presetData.name);

  auto jsonPreset = coder->encode(presetData);
  std::cout << jsonPreset << std::endl;
  createAndSavePresetFile(presetData.name, jsonPreset);

  if (auto preset = coder->decode(jsonPreset))
    presets.add(*preset);
}

void PresetManager::removePreset(String& name) {
  for (int i = 0; i < presets.size(); i++) {
    if (presets[i].name == name) {
      presetsDirectory.getChildFile(name).withFileExtension("blocks").deleteFile();
      presets.remove(i);
      break;
    }
  }
}

void PresetManager::createAndSavePresetFile(String& name, const std::string& presetJson) const {
  auto newFile = presetsDirectory.getChildFile(name).withFileExtension("blocks");
  newFile.create();
  newFile.appendText(presetJson);
}