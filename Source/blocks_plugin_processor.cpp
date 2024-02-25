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

PluginProcessor::PluginProcessor(): juce::AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)), SynthGuiInterface(this, false) {
  // for (auto module : synth.moduleManager.pool.allModules) {
  //   for (auto parameter : module->parameters) {
  //     addParameter(parameter->audioParameter);
  //     parameter->audioParameter->addListener(this);
  //   }
  // }

  // please(); exit(1);

  last_seconds_time_ = 0.0;

  int num_params = vital::Parameters::getNumParameters();
  for (int i = 0; i < num_params; ++i) {
    const vital::ValueDetails* details = vital::Parameters::getDetails(i);
    if (controls_.count(details->name) == 0)
      continue;

    ValueBridge* bridge = new ValueBridge(details->name, controls_[details->name]);
    bridge->setListener(this);
    bridge_lookup_[details->name] = bridge;
    // addParameter(bridge);
  }

  bypass_parameter_ = bridge_lookup_["bypass"];
}

PluginProcessor::~PluginProcessor() {
  midi_manager_ = nullptr;
  // keyboard_state_ = nullptr;
}

SynthGuiInterface* PluginProcessor::getGuiInterface() {
  // AudioProcessorEditor* editor = getActiveEditor();
  // if (editor)
  //   return dynamic_cast<SynthGuiInterface*>(editor);
  return nullptr;
}

void PluginProcessor::beginChangeGesture(const std::string& name) {
  if (bridge_lookup_.count(name))
    bridge_lookup_[name]->beginChangeGesture();
}

void PluginProcessor::endChangeGesture(const std::string& name) {
  if (bridge_lookup_.count(name))
    bridge_lookup_[name]->endChangeGesture();
}

void PluginProcessor::setValueNotifyHost(const std::string& name, vital::mono_float value) {
  if (bridge_lookup_.count(name)) {
    vital::mono_float plugin_value = bridge_lookup_[name]->convertToPluginValue(value);
    bridge_lookup_[name]->setValueNotifyHost(plugin_value);
  }
}

const juce::CriticalSection& PluginProcessor::getCriticalSection() {
  return getCallbackLock();
}

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
}

void PluginProcessor::releaseResources() {
}

void PluginProcessor::processBlock(AudioSampleBuffer& buffer, MidiBuffer& midi_messages) {
  keyboard_state_.processNextMidiBuffer(midi_messages, 0, buffer.getNumSamples(), true);

  static constexpr double kSecondsPerMinute = 60.0f;

  if (bypass_parameter_->getValue()) {
    processBlockBypassed(buffer, midi_messages);
    return;
  }

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
  if (block_modified_) {
    getVoiceHandler()->unplugAll();
    getVoiceHandler()->connectAll();

    clearModulations();
    auto connections = getModuleManager().getConnections();
    for (auto c : connections) {
      connectModulationFromModel(c);
    }

    getVoiceHandler()->disconnectAllDefaultEnvs();
    getVoiceHandler()->connectAllDefaultEnvs();

    block_modified_ = false;
  }
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
  mainComponent = &editor->mainComponent;
  editorReady = true;
  return editor;
}

void PluginProcessor::parameterChanged(std::string name, vital::mono_float value) {
  valueChangedExternal(name, value);
}

void PluginProcessor::getStateInformation(MemoryBlock& dest_data) {
  json data = LoadSave::stateToJson(this, getCallbackLock());
  data["tuning"] = getTuning()->stateToJson();

  String data_string = data.dump();
  MemoryOutputStream stream;
  stream.writeString(data_string);
  dest_data.append(stream.getData(), stream.getDataSize());
}

void PluginProcessor::setStateInformation(const void* data, int size_in_bytes) {
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
void PluginProcessor::editorAdjustedModulator(int parameter, int index, float value) {
  auto modulator = synth_->getModuleManager().getModulator(index);

  if (modulator->id.type == "lfo") {
    if (parameter == 1) {
      bool is_changing_seconds = modulator->parameter_map_["sync"]->val->value() == 0.0f;
      if (is_changing_seconds) {
        modulator->parameter_map_["frequency"]->val->set(value);
      } else {
        modulator->parameter_map_["tempo"]->val->set(value);
      }
    }
  }

  modulator->parameters_[parameter]->val->set(value);
}

void PluginProcessor::editorAdjustedBlock(Index index, int parameter, float value) {
  auto block = synth_->getModuleManager().getBlock(index);

  if (block->id.type == "delay") {
    std::cout << parameter << std::endl;
    if (parameter == 4) {
      auto sync = block->parameter_map_["sync"]->val->value();
      bool is_changing_seconds = block->parameter_map_["sync"]->val->value() == 0.0f;
      if (is_changing_seconds) {
        std::cout << "is changing freq: " << value << std::endl;
        block->parameter_map_["frequency"]->val->set(value);
      } else {
        std::cout << "is changing tempo" << std::endl;
        block->parameter_map_["tempo"]->val->set(value);
      }
      return;
    }
  }

  block->parameters_[parameter]->val->set(value);
}

void PluginProcessor::editorChangedModulationMagnitude(int index, float magnitude) {
  synth_->getModuleManager().getConnection(index)->amount_parameter_->val->set(magnitude);
}

void PluginProcessor::editorChangedModulationPolarity(int index, bool bipolar) {
  synth_->getModuleManager().getConnection(index)->bipolar_parameter_->val->set(bipolar);
  // moduleManager.getConnection(index)->setPolarity(bipolar);
}

void PluginProcessor::editorAdjustedTab(int column, int parameter, float value) {
  // moduleManager.getTab(column)->parameter(parameter)->setValue(value);
}

#pragma warning(default:4716)
std::shared_ptr<Module> PluginProcessor::getModulator(int index) {
  // return moduleManager.getModulator(index);
}

juce::Array<std::shared_ptr<Modulation>> PluginProcessor::getConnectionsOfSource(std::shared_ptr<Module> source) {
  return juce::Array<std::shared_ptr<Modulation>>();
  // return moduleManager.getConnectionsOfSource(source);
}

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

#pragma warning(default:4716)
std::shared_ptr<Block> PluginProcessor::getBlock(Index index) {
}

std::shared_ptr<model::Module> PluginProcessor::getBlock2(Index index) {
  return (index.row == -1 || index.column == -1) ? nullptr : synth_->getModuleManager().getBlock(index);
}

#pragma warning(default:4716)
std::shared_ptr<Tab> PluginProcessor::getTab(int column) {
  // return moduleManager.getTab(column);
}

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
  block_modified_ = true;
}

void PluginProcessor::editorConnectedModulation(int modulatorIndex, std::string target_name, std::string parameter) {
  synth_->connectModulation(modulatorIndex, target_name, parameter);
  // Analytics::shared()->countAction("Modulation Connected");
  // connect(modulatorIndex, targetName, parameter);
}

void PluginProcessor::editorDisconnectedModulation(int index) {
  auto connection = synth_->getModuleManager().getConnection(index);
  disconnect(connection);
  // Analytics::shared()->countAction("Modulation Disconnected");
}

Preset PluginProcessor::editorChangedPreset(int index) {
  // if (index == -1) {
  //   clear();
  //   return Preset();
  // }

  auto preset = preset_manager_.presets[0];
  loadPreset(preset);
  return preset;
}

void PluginProcessor::loadPreset(Preset preset) {
  clear();
  // for (auto presetTab : preset.tabs) {
  //   auto tab = moduleManager.addTab(presetTab.id.type, presetTab.column, presetTab.id.number);
  //   tab->length = presetTab.length;

  //   for (auto const& [key, val] : presetTab.parameters)
  //     tab->parameterMap[key]->audioParameter->setValue(val);
  // }

  for (auto presetBlock : preset.blocks) {
    Index index = { presetBlock.index.first, presetBlock.index.second };
    auto block = addBlock(presetBlock.id.type, index, presetBlock.id.number);
    // auto block = addBlock(

    for (auto const& [key, val] : presetBlock.parameters) {
      // block->parameterMap[key]->audioParameter->setValue(val);
      // block->parameter_map_[key]->val->set(val);
    }

    if (presetBlock.length > 1) {
      // expand(block->index, presetBlock.length - 1, true);
    }
  }

  // for (auto presetModulator : preset.modulators) {
  //   auto modulator = addModulator(presetModulator.id.type, presetModulator.id.number, presetModulator.colour);
  //   for (auto const& [key, val] : presetModulator.parameters)
  //     modulator->parameterMap[key]->audioParameter->setValue(val);
  // }

  // for (auto presetConnection : preset.connections_) {
  //   auto modulator = moduleManager.getModule(presetConnection.source);

  //   int modulatorIndex = moduleManager.modulators.indexOf(modulator);

  //   auto target = moduleManager.getModule(presetConnection.target);
  //   auto parameterName = presetConnection.parameter;
  //   auto parameter = target->parameterMap[parameterName];
  //   auto parameterIndex = target->parameters.indexOf(parameter);
  //   auto modulation = connect(modulatorIndex, presetConnection.target, parameterIndex, presetConnection.number);

  //   modulation->magnitudeParameter->setValue(presetConnection.amount);
  //   modulation->setPolarity(presetConnection.bipolar);
  // }
  block_modified_ = true;
}

void PluginProcessor::clear() {
  // pauseProcessing(true);
  // initEngine();
  clearModulations();

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
  // pauseProcessing(false);
}

void PluginProcessor::editorRemovedTab(int column) {
  // Analytics::shared()->countAction("Tab Removed");
  // removeTab(column);
}

#pragma warning(default:4716)
std::shared_ptr<Tab> PluginProcessor::editorAddedTab(int column) {
  // auto type = Model::Types::noteTab;
  // Analytics::shared()->countAction(type + " Tab Added");
  // return moduleManager.addTab(type, column, -1);
}

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
    getVoiceHandler()->setDefaultAmpEnv(connection->target->name, true);
  }
  synth_->disconnectModulation(connection->vital_connection_);
  getModuleManager().removeConnection(connection);
}

#pragma warning(default:4716)
std::shared_ptr<Module> PluginProcessor::editorAddedModulator(Model::Type code) {

}

std::shared_ptr<model::Module> PluginProcessor::editorAddedModulator2(Model::Type code) {
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

  block_modified_ = true;
}

std::shared_ptr<Block> PluginProcessor::editorAddedBlock(Model::Type type, Index index) {
  return nullptr;
}

std::shared_ptr<model::Block> PluginProcessor::editorAddedBlock2(Model::Type type, Index index) {
  auto block = synth_->addBlock(type, index);
  block_modified_ = true;
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

juce::Array<std::shared_ptr<Module>> PluginProcessor::getModulators() {
  return {};
  // Array<std::shared_ptr<Module>> array;
  // for (auto modulator : moduleManager.modulators) array.add(modulator);
  // return array;
}

void PluginProcessor::editorSavedPreset(String name) {
  // Analytics::shared()->countAction("Preset Saved");
  auto info = Preset::create(name, getModuleManager().getBlocks(), getModuleManager().getModulators(), getModuleManager().getConnections());
  std::cout << "yay" << std::endl;
  preset_manager_.save(info);
  // per
  // presetManager.save(info);
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
  // auto connection = moduleManager.getConnection(modulationConnectionIndex);
  // auto currentVoice = blockVoices[getCurrentVoiceIndex()];

  // if (auto modulationInput = currentVoice->getModulationInput(connection))
  //   return { modulationInput->source->getValue(0), modulationInput->connection->getMagnitude() };

  // return { 0.0f, 0.0f };
}

#pragma warning(default:4716)
Preset PluginProcessor::getStateRepresentation() {
  // auto currentState = PresetInfo();

  // for (auto block : moduleManager.getBlocks()) {
  //   auto presetBlock = PresetInfo::Block();
  //   presetBlock.index = { block->index.row, block->index.column }; ;
  //   presetBlock.id = block->id;
  //   presetBlock.length = block->length;
  //   currentState.blocks.add(presetBlock);
  // }

  // for (auto tab : moduleManager.getTabs()) {
  //   auto presetTab = PresetInfo::Tab();
  //   presetTab.column = tab->column;
  //   presetTab.id = tab->id;
  //   currentState.tabs.add(presetTab);
  // }

  // return currentState;
}

juce::StringArray PluginProcessor::editorRequestsPresetNames() {
  // StringArray result;

  // for (auto preset : presetManager.presets)
  //   result.add(preset.name);

  return {};
}

const vital::StatusOutput* PluginProcessor::editorRequestsStatusOutput(std::string name) {
  return getStatusOutput(name);
}

// MainComponent::Delegate end 
void PluginProcessor::editorParameterGestureChanged(String moduleName, int parameterIndex, bool started) {
  std::cout << moduleName.toStdString() << " is changing " << std::endl;

  if (JUCE_STANDALONE_APPLICATION) return;

  if (started) {
    // synth_->BeginParameterChangeGesture(moduleName, parameterIndex);
  // moduleManager.getModule(moduleName)->parameter(parameterIndex)->audioParameter->beginChangeGesture();
  } else {
    // moduleManager.getModule(moduleName)->parameter(parameterIndex)->audioParameter->endChangeGesture();
  }
}

std::vector<std::shared_ptr<model::Module>> PluginProcessor::getModulators2() {
  return synth_->getModuleManager().getModulators();
}