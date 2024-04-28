/* Copyright 2013-2019 Matt Tytel
 *
 * pylon is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * pylon is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with pylon.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "blocks_plugin_processor.h"
#include "PluginEditor.h"

#include "vital/synthesis/synth_engine/sound_engine.h"
#include "vital/common/load_save.h"
#include "vital/synthesis/modules/blocks_voice_handler.h"
#include "playground.h"
#include "util/Analytics.h"

PluginProcessor::PluginProcessor(): juce::AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)), SynthGuiInterface(this, false) {

  // for getVoiceHandler()->column_nodes_
  for (auto column_control : synth_->getModuleManager().pool.column_controls_) {
    auto processor = getVoiceHandler()->column_controls_[column_control->id.number - 1];
    for (auto& pair : column_control->parameter_map_) {
      column_control->parameter_map_[pair.first]->value_processor = processor->control_map_[pair.first];
    }
  }

  Analytics::shared()->initProfileIfNeeded();
  Analytics::shared()->handleLaunch(getWrapperTypeDescription(wrapperType));

  // for (auto module : getModuleManager().pool.all_modules_) {
  //   for (auto parameter : module->parameters_) {
  //     ValueBridge* bridge = new ValueBridge(*(parameter.get()));
  //     // bridge->
  //     bridge->setListener(this);
  //     auto key = module->name + " " + parameter->name;
  //     bridge_lookup_[key] = bridge;
  //     // parameter->bridge = bridge;
  //     addParameter(bridge);
  //   }
  // }

  // for (auto module : synth.moduleManager.pool.allModules) {
  //   for (auto parameter : module->parameters) {
      // parameter>val
      // addParameter(parameter->audioParameter);
      // parameter->audioParameter->addListener(this);
  //   }
  // }

  // please(); exit(1);

  last_seconds_time_ = 0.0;


  // int num_params = vital::Parameters::getNumParameters();
  // for (int i = 0; i < num_params; ++i) {
  //   const vital::ValueDetails* details = vital::Parameters::getDetails(i);
  //   if (controls_.count(details->name) == 0)
  //     continue;

  //   ValueBridge* bridge = new ValueBridge(details);
  //   bridge->setListener(this);
  //   bridge_lookup_[details->name] = bridge;
    // addParameter(bridge);
  // }

  // bypass_parameter_ = bridge_lookup_["bypass"];
}

PluginProcessor::~PluginProcessor() {
  midi_manager_ = nullptr;
  Analytics::shared()->handleQuit();
  // keyboard_state_ = nullptr;
}

SynthGuiInterface* PluginProcessor::getGuiInterface() {

  // AudioProcessorEditor* editor = getActiveEditor();
  // if (editor)
  //   return dynamic_cast<SynthGuiInterface*>(editor);
  return nullptr;
}

void PluginProcessor::beginChangeGesture(const std::string& name) {
  if (bridge_lookup_.count(name)) bridge_lookup_[name]->beginChangeGesture();
}

void PluginProcessor::endChangeGesture(const std::string& name) {
  if (bridge_lookup_.count(name)) bridge_lookup_[name]->endChangeGesture();
}

void PluginProcessor::setValueNotifyHost(const std::string& name, vital::mono_float value) {
  // if (bridge_lookup_.count(name)) {
  //   vital::mono_float plugin_value = bridge_lookup_[name]->convertToPluginValue(value);
  //   bridge_lookup_[name]->setValueNotifyHost(plugin_value);
  // }
}

// const juce::CriticalSection& PluginProcessor::getCriticalSection() {
//   return getCallbackLock();
// }

void PluginProcessor::pauseProcessing(bool pause) {
  suspendProcessing(pause);
}

const String PluginProcessor::getName() const {
  return JucePlugin_Name;
}

const String PluginProcessor::getInputChannelName(int channel_index) const {
  return String(channel_index + 1);
}

const String PluginProcessor::getOutputChannelName(int channel_index) const {
  return String(channel_index + 1);
}

bool PluginProcessor::isInputChannelStereoPair(int index) const {
  return true;
}

bool PluginProcessor::isOutputChannelStereoPair(int index) const {
  return true;
}

bool PluginProcessor::acceptsMidi() const {
#if JucePlugin_WantsMidiInput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::producesMidi() const {
#if JucePlugin_ProducesMidiOutput
  return true;
#else
  return false;
#endif
}

bool PluginProcessor::silenceInProducesSilenceOut() const {
  return false;
}

double PluginProcessor::getTailLengthSeconds() const {
  return 0.0;
}

const String PluginProcessor::getProgramName(int index) {
  // SynthGuiInterface* editor = getGuiInterface();
  // if (editor == nullptr || editor->getSynth() == nullptr)
  //   return "";

  return "hep";//editor->getSynth()->getPresetName();
}

void PluginProcessor::prepareToPlay(double sample_rate, int buffer_size) {
  engine_->setSampleRate(sample_rate);
  engine_->updateAllModulationSwitches();
  midi_manager_->setSampleRate(sample_rate);
  engine_prepared_ = true;
  if (pending_preset_) {
    loadPreset(*pending_preset_);
    pending_preset_ = std::nullopt;
  } else if (auto preset = preset_manager_.stringToPreset(getStateString())) {
    loadPreset(*preset);
  }
}

void PluginProcessor::releaseResources() {
}

void PluginProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midi_messages) {
  keyboard_state_.processNextMidiBuffer(midi_messages, 0, buffer.getNumSamples(), true);

  static constexpr double kSecondsPerMinute = 60.0f;

  // if (bypass_parameter_->getValue()) {
  //   processBlockBypassed(buffer, midi_messages);
  //   return;
  // }

  int total_samples = buffer.getNumSamples();
  int num_channels = getTotalNumOutputChannels();
  if (const auto position_info_ = getPlayHead()->getPosition()) {
    if (position_info_->getBpm())
      engine_->setBpm(*position_info_->getBpm());

    if (position_info_->getIsPlaying()) {
      double bps = (*position_info_->getBpm()) / kSecondsPerMinute;
      last_seconds_time_ = (*position_info_->getPpqPosition()) / bps;
    }
  }

  handleBlockChanges();
  processModulationChanges();

  if (total_samples)
    processKeyboardEvents(midi_messages, total_samples);

  double sample_time = 1.0 / AudioProcessor::getSampleRate();
  for (int sample_offset = 0; sample_offset < total_samples;) {
    int num_samples = std::min<int>(total_samples - sample_offset, vital::kMaxBufferSize);

    engine_->correctToTime(last_seconds_time_);
    processMidi(midi_messages, sample_offset, sample_offset + num_samples);
    processAudio(&buffer, num_channels, num_samples, sample_offset);

    last_seconds_time_ += num_samples * sample_time;
    sample_offset += num_samples;
  }
}

void PluginProcessor::handleBlockChanges() {
  if (!block_updates_) return;

  getVoiceHandler()->unplugAll();
  getVoiceHandler()->connectAll();

  clearModulations();
  auto connections = getModuleManager().getConnections();
  for (auto c : connections) {
    connectModulationFromModel(c);
  }

  getVoiceHandler()->disconnectAllDefaultEnvs();
  getVoiceHandler()->connectAllDefaultEnvs();

  block_updates_--;
}

bool PluginProcessor::hasEditor() const {
  return true;
}

juce::AudioProcessorEditor* PluginProcessor::createEditor() {
  auto editor = new PluginEditor(keyboard_state_, *this);
  if (TopLevelWindow::getNumTopLevelWindows() == 1) {
    TopLevelWindow* w = TopLevelWindow::getTopLevelWindow(0);
    w->setUsingNativeTitleBar(true);
  }
  main_component_ = &editor->mainComponent;
  editor_ready_ = true;
  return editor;
}

void PluginProcessor::parameterChanged(std::string name, vital::mono_float value) {
  valueChangedExternal(name, value);
}

std::string PluginProcessor::getStateString() {
  auto columns = getModuleManager().pool.column_controls_;
  auto info = Preset::create("", getModuleManager().getBlocks(), getModuleManager().getModulators(), getModuleManager().getConnections(), columns);
  auto s = preset_manager_.presetToString(info);
  return s;
}

void PluginProcessor::getStateInformation(MemoryBlock& dest_data) {
  std::string state = getStateString();
  dest_data.replaceAll(state.data(), state.size());

  // json data = LoadSave::stateToJson(this, getCallbackLock());
  // data["tuning"] = getTuning()->stateToJson();

  // String data_string = data.dump();
  // MemoryOutputStream stream;
  // stream.writeString(data_string);
  // dest_data.append(stream.getData(), stream.getDataSize());
}

void PluginProcessor::setStateInformation(const void* data, int size_in_bytes) {
  // return;
  auto preset_string = String::fromUTF8(static_cast<const char*> (data), size_in_bytes);
  // std::
  if (auto preset = preset_manager_.stringToPreset(preset_string.toStdString())) {
    if (engine_prepared_) {
      loadPreset(*preset);
      if (editor_ready_) main_component_->loadState(*preset);
    } else {
      pending_preset_ = preset;
      // synth.presetToLoadOnInit = *preset;
    }
  }
  // juce::MemoryInputStream stream(data, size_in_bytes, false);
  // String data_string = stream.readEntireStreamAsString();

  // pauseProcessing(true);
  // try {
  //   json json_data = json::parse(data_string.toStdString());
  //   LoadSave::jsonToState(this, save_info_, json_data);

  //   if (json_data.count("tuning"))
  //     getTuning()->jsonToState(json_data["tuning"]);
  // } catch (const json::exception& e) {
  //   std::string error = "There was an error open the preset. Preset file is corrupted.";
  //   juce::AlertWindow::showNativeDialogBox("Error opening preset", error, false);
  // }
  // pauseProcessing(false);

  // SynthGuiInterface* editor = getGuiInterface();
  // if (editor)
  //   editor->updateFullGui();
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new PluginProcessor();
}

// MainComponent::Delegate
void PluginProcessor::editorAdjustedModulator(std::string parameter_name, int index, float value) {
  auto modulator = synth_->getModuleManager().getModulator(index);
  modulator->parameter_map_[modulator->getParameterName(parameter_name)]->set(value);
}

void PluginProcessor::editorAdjustedBlock(Index index, int parameter, float value) {
  auto block = synth_->getModuleManager().getBlock(index);

  if (block->id.type == "delay") {
    if (parameter == 4 || parameter == 6) {
      auto sync_parameter_name = parameter == 4 ? "sync" : "sync 2";
      auto sync = block->parameter_map_[sync_parameter_name]->value_processor->value();
      bool is_changing_seconds = block->parameter_map_[sync_parameter_name]->value_processor->value() == 0.0f;
      std::string name = is_changing_seconds ? "frequency" : "tempo";
      if (parameter == 6) name += " 2";
      block->parameter_map_[name]->value_processor->set(value);
      return;
    }
  }

  bool is_changing_delay_tempo = block->id.type == "delay" && parameter == 4;
  bool is_changing_mod_tempo = (block->id.type == "phaser" || block->id.type == "chorus" || block->id.type == "flanger") && parameter == 3;
  if (is_changing_delay_tempo || is_changing_mod_tempo) {
    auto sync = block->parameter_map_["sync"]->value_processor->value();
    bool is_changing_seconds = block->parameter_map_["sync"]->value_processor->value() == 0.0f;
    if (is_changing_seconds) {
      block->parameter_map_["frequency"]->value_processor->set(value);
    } else {
      block->parameter_map_["tempo"]->value_processor->set(value);
    }
    return;
  }

  block->parameters_[parameter]->set(value);
}

void PluginProcessor::editorChangedModulationMagnitude(int index, float magnitude) {
  synth_->getModuleManager().getConnection(index)->amount_parameter_->set(magnitude);
}

void PluginProcessor::editorChangedModulationPolarity(int index, bool bipolar) {
  synth_->getModuleManager().getConnection(index)->bipolar_parameter_->set(bipolar);
  // moduleManager.getConnection(index)->setPolarity(bipolar);
}

void PluginProcessor::editorAdjustedTab(int column, int parameter, float value) {
  // moduleManager.getTab(column)->parameter(parameter)->setValue(value);
}

std::shared_ptr<model::Module> PluginProcessor::getModulator2(int index) {
  return getModuleManager().getModulator(index);
}

// juce::Array<std::shared_ptr<Modulation>> PluginProcessor::getConnectionsOfSource(std::shared_ptr<Module> source) {
//   return juce::Array<std::shared_ptr<Modulation>>();
// }

std::vector<std::shared_ptr<model::Connection>> PluginProcessor::getModulations() {
  return synth_->getModuleManager().getConnections();
}

// std::vector<std::shared_ptr<model::Modulation>> PluginProcessor::getModulations2() {
//   std::vector<std::shared_ptr<model::Modulation>> modulations;

//   auto& bank = synth_->getModulationBank();
//   for (int i = 0; i < vital::kMaxModulationConnections; i++) {
//     auto mod = bank.atIndex(i);

//     bool is_connection_active = mod->destination_name != "" && mod->source_name != "";
//     if (!is_connection_active) continue;

//     auto modulation = std::make_shared<model::Modulation>(nullptr, nullptr, 0, 1.0f, i, false);
//     modulations.push_back(modulation);
//   }

//   return modulations;
// }

std::shared_ptr<model::Block> PluginProcessor::getBlock2(Index index) {
  return (index.row == -1 || index.column == -1) ? nullptr : synth_->getModuleManager().getBlock(index);
}

// #pragma warning(default:4716)
// std::shared_ptr<Tab> PluginProcessor::getTab(int column) {
  // return moduleManager.getTab(column);
// }

#pragma warning(default:4716)
juce::Array<MPENote> PluginProcessor::editorRequestsCurrentlyPlayingNotes() {
  // return currentlyPlayingNotes;
}

#pragma warning(default:4716)
juce::Array<int> PluginProcessor::editorRequestsActiveColumns() {
  // return moduleManager.getActiveColumns();
}

void PluginProcessor::editorRepositionedBlock(Index from, Index to) {
  // clearModulations();
  synth_->repositionBlock(from, to);
  auto block = synth_->getModuleManager().getBlock(to);
  block_updates_++;
}

void PluginProcessor::editorConnectedModulation(int modulatorIndex, std::string target_name, std::string parameter) {
  synth_->connectModulation(modulatorIndex, target_name, parameter);
}

void PluginProcessor::editorDisconnectedModulation(int modulator_index, std::string target_name, std::string parameter) {
  auto connection = synth_->getModuleManager().getConnection(modulator_index, target_name, parameter);
  disconnect(connection);
}

void PluginProcessor::editorDisconnectedModulation(int index) {
  auto connection = synth_->getModuleManager().getConnection(index);
  disconnect(connection);
  // Analytics::shared()->countAction("Modulation Disconnected");
}

Preset PluginProcessor::editorChangedPreset(int index) {
  if (index == -1) {
    clear();
    current_preset_index_ = -1;
    return Preset();
  }

  auto preset = preset_manager_.presets[index];
  loadPreset(preset);
  return preset;
}

void PluginProcessor::loadPreset(Preset preset) {
  pauseProcessing(true);
  engine_->allSoundsOff();
  clear();

  // tabs will come back soon 
  // for (auto presetTab : preset.tabs) {
  //   auto tab = moduleManager.addTab(presetTab.id.type, presetTab.column, presetTab.id.number);
  //   tab->length = presetTab.length;
  //   for (auto const& [key, val] : presetTab.parameters)
  //     tab->parameterMap[key]->audioParameter->setValue(val);
  // }

  for (auto presetBlock : preset.blocks) {
    Index index = { presetBlock.index.first, presetBlock.index.second };
    auto block = addBlock(presetBlock.id.type, index, presetBlock.id.number);

    for (auto const& [key, val] : presetBlock.parameters) {
      block->parameter_map_[key]->set(val);
    }

    // expanding will come back soon 
    // if (presetBlock.length > 1) {
      // expand(block->index, presetBlock.length - 1, true);
    // }
  }

  for (auto presetModulator : preset.modulators) {
    auto modulator = addModulator(presetModulator.id.type, presetModulator.id.number, presetModulator.colour);
    for (auto const& [key, val] : presetModulator.parameters)
      modulator->parameter_map_[key]->set(val);
  }

  for (auto presetConnection : preset.connections_) {
    auto modulator = synth_->getModuleManager().getModule(presetConnection.source);

    auto target = synth_->getModuleManager().getModule(presetConnection.target);
    auto model = synth_->getModuleManager().addConnection(modulator, target, presetConnection.parameter, presetConnection.number);
    model->amount_parameter_->value = presetConnection.amount;
    model->bipolar_parameter_->value = presetConnection.bipolar;
    connectModulationFromModel(model);
  }

  for (auto column_control : preset.column_controls) {
    auto index = column_control.id.number - 1;
    for (auto const& [key, val] : column_control.parameters) {
      getModuleManager().pool.column_controls_[index]->parameter_map_[key]->set(val);
    }
  }

  block_updates_++;
  pauseProcessing(false);
}

void PluginProcessor::clear() {
  pauseProcessing(true);
  // initEngine();
  clearModulations();

  for (auto column_control : getModuleManager().pool.column_controls_) {
    for (auto const& [key, val] : column_control->parameter_map_) {
      column_control->parameter_map_[key]->value_processor->set(column_control->parameter_map_[key]->default_value);
    }
  }

  auto num_modulators = synth_->getModuleManager().getModulators().size();
  for (int i = num_modulators - 1; i >= 0; i--) {
    removeModulator(i);
  }

  auto blocks = synth_->getModuleManager().getBlocks();
  for (int i = blocks.size() - 1; i >= 0; i--) {
    removeBlock(blocks[i]->index);
  }

  getModuleManager().clear();
  getVoiceHandler()->clear();
  pauseProcessing(false);
}

void PluginProcessor::editorRemovedTab(int column) {
  // Analytics::shared()->countAction("Tab Removed");
  // removeTab(column);
}

// #pragma warning(default:4716)
// std::shared_ptr<Tab> PluginProcessor::editorAddedTab(int column) {
  // auto type = Model::Types::noteTab;
  // Analytics::shared()->countAction(type + " Tab Added");
  // return moduleManager.addTab(type, column, -1);
// }

void PluginProcessor::editorRemovedModulator(int index) {
  removeModulator(index);
}

void PluginProcessor::removeModulator(int index) {
  auto modulator = synth_->getModuleManager().getModulator(index);

  auto module_connections = synth_->getModuleManager().getConnectionsOfSource(modulator);

  for (auto connection : module_connections) {
    disconnect(connection);
  }

  getVoiceHandler()->removeModulator(index, modulator->id.type, modulator->name);
  getModuleManager().removeModulator(index);
}

void PluginProcessor::disconnect(std::shared_ptr<model::Connection>& connection) {
  bool disconnecting_osc_env = connection->source->id.type == "envelope" && connection->target->id.type == "osc" && connection->parameter_name_ == "level";
  if (disconnecting_osc_env) {
    getVoiceHandler()->setDefaultAmpEnvState(connection->target->name, true);
  }

  printf("disconnecting %s %s %s\n", connection->source->name.c_str(), connection->target->name.c_str(), connection->parameter_name_.c_str());
  synth_->disconnectModulation(connection->vital_connection_);
  getModuleManager().removeConnection(connection);
}

std::shared_ptr<model::Module> PluginProcessor::editorAddedModulator2(std::string code) {
  return synth_->addModulator(code);
}

void PluginProcessor::editorRemovedBlock(Index index) {
  // Analytics::shared()->countAction("Block Removed");
  removeBlock(index);
}

void PluginProcessor::removeBlock(const Index& index) {
  auto block = synth_->getModuleManager().getBlock(index);

  for (auto connection : synth_->getModuleManager().getConnectionsOfSource(block)) {
    disconnect(connection);
  }

  getVoiceHandler()->removeBlock(index, block);
  synth_->getModuleManager().removeBlock(block);

  block_updates_++;
}

std::shared_ptr<model::Block> PluginProcessor::editorAddedBlock2(std::string type, Index index) {
  auto block = synth_->addBlock(type, index);
  block_updates_++;
  return block;
}

// std::shared_ptr<model::Block> PluginProcessor::editorAddedBlock3(Model::Type type, Index index) {
//   vital::block_change change;
//   change.index = index;
//   change.type = BlockChangeType::kAdd;
//   auto block = module_manager_.addBlock(type, index);


//   block_change_queue_.enqueue(change);

//   return block;
// }

void PluginProcessor::editorRepositionedTab(int oldColumn, int newColumn) {
  // Analytics::shared()->countAction("Tab Repositioned");

  // auto tab = moduleManager.getTab(oldColumn);
  // auto columns = tab->getAllColumns();
  // auto currentBlockVoice = blockVoices[getCurrentVoiceIndex()];
  // currentBlockVoice->setColumnsState(columns, false);

  // tab->column = newColumn;
}

void PluginProcessor::editorChangedTabLength(int column, int length) {
  // Analytics::shared()->countAction("Tab Length Changed");
  // auto tab = moduleManager.getTab(column);
  // tab->length = length;
}

void PluginProcessor::editorChangedBlockLength(Index index, int length) {
  // Analytics::shared()->countAction("Block Length Changed");
  // auto parent = moduleManager.getBlock(index);
  // auto difference = length - parent->length;
  // bool expanded = difference > 0;

  // if (expanded) {
  //   expand(index, difference, true);
  // } else {
  //   reduce(index, abs(difference));
  // }
}

void PluginProcessor::editorSavedPreset(std::string name) {
  auto columns = getModuleManager().pool.column_controls_;
  auto info = Preset::create(name, getModuleManager().getBlocks(), getModuleManager().getModulators(), getModuleManager().getConnections(), columns);
  preset_manager_.save(info);
  current_preset_index_ = preset_manager_.presets.size() - 1;
}

#pragma warning(default:4716)
std::pair<float, float> PluginProcessor::editorRequestsModulatorValue(Index moduleIndex, int parameterIndex, int modulatorIndex) {
  // auto voice = blockVoices[getCurrentVoiceIndex()];
  // auto processor = voice->getProcessor(moduleIndex);
  // auto parameter = processor->getParameter(parameterIndex);
  // auto modulationInput = parameter->modulationInputs[modulatorIndex];

  // if (modulationInput)
  //   return { modulationInput->source->getValue(0), modulationInput->connection->getMagnitude() };

  // return { 0.0f, 0.0f };
}

#pragma warning(default:4716)
std::pair<float, float> PluginProcessor::editorRequestsModulatorValue(int modulationConnectionIndex) {
  auto connection = getModuleManager().getConnection(modulationConnectionIndex);
  // auto connection = moduleManager.getConnection(modulationConnectionIndex);
  // auto currentVoice = blockVoices[getCurrentVoiceIndex()];

  // if (auto modulationInput = currentVoice->getModulationInput(connection))
  //   return { modulationInput->source->getValue(0), modulationInput->connection->getMagnitude() };

  // return { 0.0f, 0.0f };
}

#pragma warning(default:4716)
Preset PluginProcessor::getStateRepresentation() {
  auto current_state = Preset();

  for (auto block : getModuleManager().getBlocks()) {
    auto preset_block = Preset::Block();
    preset_block.index = { block->index.column, block->index.row }; ;
    preset_block.id = block->id;
    preset_block.length = block->length;
    current_state.blocks.push_back(preset_block);
  }

  for (auto column_control : getModuleManager().pool.column_controls_) {
    auto preset_column_control = Preset::Module();
    preset_column_control.id = column_control->id;
    for (auto parameter : column_control->parameter_map_) {
      preset_column_control.parameters[parameter.first] = parameter.second->value_processor->value();
    }
    current_state.column_controls.push_back(preset_column_control);
  }

  // for (auto tab : moduleManager.getTabs()) {
  //   auto presetTab = PresetInfo::Tab();
  //   presetTab.column = tab->column;
  //   presetTab.id = tab->id;
  //   currentState.tabs.add(presetTab);
  // }

  return current_state;
}

StringArray PluginProcessor::editorRequestsPresetNames() {
  StringArray result;
  for (auto preset : preset_manager_.presets) result.add(preset.name);
  return result;
}

const vital::StatusOutput* PluginProcessor::editorRequestsStatusOutput(std::string name) {
  return getStatusOutput(name);
}

// MainComponent::Delegate end 
void PluginProcessor::editorParameterGestureChanged(std::string module_name, std::string parameter_name, bool started) {
  if (JUCE_STANDALONE_APPLICATION) return;
  auto key = module_name + " " + parameter_name;
  if (started) {
    bridge_lookup_[key]->beginChangeGesture();
  } else {
    bridge_lookup_[key]->endChangeGesture();
  }
}

std::vector<std::shared_ptr<model::Module>> PluginProcessor::getModulators2() {
  return synth_->getModuleManager().getModulators();
}

void PluginProcessor::editorStartedAdjustingColumn(std::string control, int column) {

}

void PluginProcessor::editorEndedAdjustingColumn(std::string control, int column) {

}

void PluginProcessor::editorAdjustedColumn(std::string control, int column, float value) {
  getModuleManager().getColumnControl(column)->parameter_map_[control]->value_processor->set(value);
}

void PluginProcessor::setValue(std::string module_id, std::string parameter_name, float value) {
  synth_->getModuleManager().getModule(module_id)->parameter_map_[parameter_name]->value_processor->set(value);
}

std::optional<Preset> PluginProcessor::editorNavigatedPreset(bool next) {
  if (preset_manager_.presets.size() == 0) return {};
  int previous_index = current_preset_index_;
  if (next) {
    if (++current_preset_index_ >= preset_manager_.presets.size()) current_preset_index_ = 0;
  } else {
    if (--current_preset_index_ < 0) current_preset_index_ = preset_manager_.presets.size() - 1;
  }
  if (current_preset_index_ == previous_index) return {};
  auto preset = preset_manager_.presets[current_preset_index_];
  loadPreset(preset);
  return preset;
}

std::vector<std::shared_ptr<model::Block>> PluginProcessor::editorPastedIndices(const std::vector<model::Block> copied_blocks, Index target) {
  Index top_left_index_of_section = copied_blocks[0].index;
  for (model::Block block : copied_blocks) {
    auto index = block.index;
    if (block.index.column < top_left_index_of_section.column) top_left_index_of_section.column = block.index.column;
    if (block.index.row < top_left_index_of_section.row) top_left_index_of_section.row = block.index.row;
  }
  Index delta = target - top_left_index_of_section;
  std::vector<std::shared_ptr<model::Block>> new_blocks;
  for (model::Block copied_block : copied_blocks) {
    Index index = copied_block.index;
    Index adjusted_index = index + delta;
    auto new_block = synth_->addBlock(copied_block.id.type, adjusted_index);
    if (!new_block) continue;

    // block might be moved/modified between copy and paste
    auto copied_block_now = synth_->getModuleManager().getModule(copied_block.name);
    if (!copied_block_now) continue;

    for (auto const& [key, val] : copied_block_now->parameter_map_) {
      new_block->parameter_map_[key]->set(val->value_processor->value());
    }

    new_blocks.push_back(new_block);
    for (auto connection : getModuleManager().getConnectionsOfTarget(copied_block_now)) {
      auto modulator = synth_->getModuleManager().getModule(connection->source->name);
      auto target = synth_->getModuleManager().getModule(new_block->name);
      auto model = synth_->getModuleManager().addConnection(modulator, target, connection->parameter_name_);
      model->amount_parameter_->value = connection->amount_parameter_->value;
      model->bipolar_parameter_->value = connection->bipolar_parameter_->value;
      connectModulationFromModel(model);
    }
  }
  block_updates_++;
  return new_blocks;
}

void PluginProcessor::editorStartedAdjustingParameter(ID& id, std::string& parameter_name, bool started) {

}

void PluginProcessor::editorAdjustedParameter(ID& id, std::string& parameter_name, float value) {
  getModuleManager().getModule(id)->parameter_map_[parameter_name]->value_processor->set(value);
}

// std::shared_ptr<model::Module>& PluginProcessor::editorRequestsColumnControls() { 
//   return getModuleManager().pool.column_controls_;
// }