/*
  ==============================================================================

    PresetCoder.cpp
    Created: 30 May 2021 6:44:48pm
    Author:  Dan German

  ==============================================================================
*/

#include "PresetCoder.h"

std::string PresetCoder::encode(PresetInfo presetData) {
  json jsonPreset;

  jsonPreset["name"] = presetData.name.toStdString();
  jsonPreset["tabs"] = encodeTabs(presetData.tabs);
  jsonPreset["blocks"] = encodeBlocks(presetData.blocks);
  jsonPreset["modulators"] = encodeModulators(presetData.modulators);
  jsonPreset["modulations"] = encodeModulations(presetData.modulations);
  jsonPreset["format_version"] = 0;

  return jsonPreset.dump(2);
}

json PresetCoder::encodeModulators(Array<PresetInfo::Modulator> modulators) {
  json array = json::array();

  for (auto module : modulators) {
    json jsonModule = encodeModule(module);
    jsonModule["color"] = module.colour;
    array.push_back(jsonModule);
  }

  return array;
}

json PresetCoder::encodeTabs(Array<PresetInfo::Tab> modules) {
  json array = json::array();

  for (auto module : modules) {
    json jsonTab = encodeModule(module);
    jsonTab["column"] = module.column;
    jsonTab["length"] = module.length;
    array.push_back(jsonTab);
  }

  return array;
}

json PresetCoder::encodeBlocks(Array<PresetInfo::Block> blocks) {
  json array = json::array();

  for (auto block : blocks) {
    json jsonModule = encodeModule(block);
    jsonModule["length"] = block.length;
    jsonModule["index"] = block.index;
    array.push_back(jsonModule);
  }

  return array;
}

json PresetCoder::encodeModulations(Array<PresetInfo::Modulation> modulationConnections) {
  json array = json::array();

  for (auto modulationConnection : modulationConnections) {
    json modulation;

    modulation["source"] = modulationConnection.source;
    modulation["target"] = modulationConnection.target;
    modulation["magnitude"] = modulationConnection.magnitude;
    modulation["bipolar"] = modulationConnection.bipolar;
    modulation["parameter"] = modulationConnection.parameter;
    modulation["number"] = modulationConnection.number;

    array.push_back(modulation);
  }

  return array;
}

std::optional<PresetInfo> PresetCoder::decode(std::string jsonString) {
  json presetInJson = json::parse(jsonString);

  int version;
  try {
    version = presetInJson.at("format_version");
  } catch (json::type_error& error) {
    return { };
  }

  PresetInfo preset = PresetInfo();

  preset.name = std::string(presetInJson.at("name"));

  for (json& jsonTab : presetInJson.at("tabs")) {
    PresetInfo::Tab presetTab {};
    decodeModule(jsonTab, presetTab);
    presetTab.length = jsonTab.at("length");
    presetTab.column = jsonTab.at("column");
    preset.tabs.add(presetTab);
  }

  for (json& jsonBlock : presetInJson.at("blocks")) {
    PresetInfo::Block presetBlock {};
    decodeModule(jsonBlock, presetBlock);

    presetBlock.length = jsonBlock.at("length");
    presetBlock.index = jsonBlock.at("index");
    preset.blocks.add(presetBlock);
  }

  for (json& modulator : presetInJson.at("modulators")) {
    PresetInfo::Modulator module {};
    decodeModule(modulator, module);
    module.colour = modulator.at("color");
    preset.modulators.add(module);
  }

  for (json& jsonModulation : presetInJson.at("modulations")) {
    PresetInfo::Modulation modulation;
    modulation.target = jsonModulation.at("target");
    modulation.source = jsonModulation.at("source");
    modulation.magnitude = jsonModulation.at("magnitude");
    modulation.bipolar = bool(jsonModulation.at("bipolar"));
    modulation.parameter = jsonModulation.at("parameter");
    modulation.number = jsonModulation.at("number");

    preset.modulations.add(modulation);
  }

  return preset;
}

void PresetCoder::decodeModule(json& moduleJson, PresetInfo::Module& module) {
  auto name = moduleJson.at("name").get<std::string>();

  auto spaceIndex = name.find(' ');
  module.id.type = name.substr(0, spaceIndex);
  module.id.number = std::stoi(name.substr(spaceIndex + 1));

  for (auto& [key, value] : moduleJson.at("parameters").items())
    module.parameters[key] = value;
}

json PresetCoder::encodeModule(PresetInfo::Module module) {
  json moduleJson;
  moduleJson["name"] = module.id.type + " " + std::to_string(module.id.number);

  json parameters;
  for (auto const& [key, value] : module.parameters)
    parameters[key.toStdString()] = value;

  moduleJson["parameters"] = parameters;
  return moduleJson;
}