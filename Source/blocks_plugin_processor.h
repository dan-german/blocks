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

#pragma once

#include <juce_core/juce_core.h>

#include "vital/common/synth_base.h"
#include "vital/value_bridge.h"
#include "MainComponent.h"
#include "vital/common/synth_gui_interface.h"
#include "module_new.h"
#include "connection.h"
#include "model/PresetManager.h"

class ValueBridge;

class PluginProcessor: public SynthBase, public juce::AudioProcessor, public ValueBridge::Listener, public MainComponent::Delegate, public SynthGuiInterface {
public:
  static constexpr int kSetProgramWaitMilliseconds = 500;

  PluginProcessor();
  virtual ~PluginProcessor();

  SynthGuiInterface* getGuiInterface() override;
  void beginChangeGesture(const std::string& name) override;
  void endChangeGesture(const std::string& name) override;
  void setValueNotifyHost(const std::string& name, vital::mono_float value) override;
  const juce::CriticalSection& getCriticalSection() override;
  void pauseProcessing(bool pause) override;

  void prepareToPlay(double sample_rate, int buffer_size) override;
  void releaseResources() override;
  void processBlock(AudioSampleBuffer&, MidiBuffer&) override;

  void handleBlockChanges();

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const String getName() const override;
  bool supportsMPE() const override { return true; }

  const String getInputChannelName(int channel_index) const override;
  const String getOutputChannelName(int channel_index) const override;
  bool isInputChannelStereoPair(int index) const override;
  bool isOutputChannelStereoPair(int index) const override;

  bool acceptsMidi() const override;
  bool producesMidi() const override;
  bool silenceInProducesSilenceOut() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override { return 1; }
  int getCurrentProgram() override { return 0; }
  void setCurrentProgram(int index) override { }
  const String getProgramName(int index) override;
  void changeProgramName(int index, const String& new_name) override { }

  void getStateInformation(juce::MemoryBlock& destData) override;
  void setStateInformation(const void* data, int size_in_bytes) override;
  juce::AudioProcessorParameter* getBypassParameter() const override { return bypass_parameter_; }

  void parameterChanged(std::string name, vital::mono_float value) override;

  // MainComponent::Delegate
  std::shared_ptr<Tab> editorAddedTab(int column) override;
  void editorRepositionedTab(int oldColumn, int newColumn) override;
  void editorChangedTabLength(int column, int length) override;
  juce::Array<int> editorRequestsActiveColumns() override;

  void editorRemovedTab(int column) override;
  void editorRemovedBlock(Index index) override;
  void removeBlock(const Index& index);
  void editorRepositionedBlock(Index from, Index to) override;
  void editorAdjustedBlock(Index index, int parameter, float value) override;
  void editorAdjustedTab(int column, int parameter, float value) override;
  void editorChangedModulationMagnitude(int connectionIndex, float magnitude) override;
  void editorParameterGestureChanged(std::string module_name, std::string paramter_name, bool started) override;
  void editorChangedModulationPolarity(int index, bool bipolar) override;
  void editorDisconnectedModulation(int index) override;
  void editorSavedPreset(std::string name) override;
  void editorConnectedModulation(int modulatorIndex, std::string target_name, std::string parameter) override;
  void editorChangedBlockLength(Index index, int length) override;
  void editorAdjustedModulator(std::string parameter_name, int modulator, float value) override;
  void editorRemovedModulator(int index) override;
  void removeModulator(int index);

  void editorStartedAdjustingColumn(std::string control, int column) override;
  void editorEndedAdjustingColumn(std::string control, int column) override;
  void editorAdjustedColumn(std::string contorl, int column, float value) override;

  void disconnect(std::shared_ptr<model::Connection>& connection);

  std::shared_ptr<Block> getBlock(Index index) override;
  std::shared_ptr<model::Block> getBlock2(Index index) override;
  std::shared_ptr<Tab> getTab(int column) override;
  std::shared_ptr<Block> editorAddedBlock(Model::Type code, Index index) override;
  std::shared_ptr<model::Block> editorAddedBlock2(Model::Type code, Index index) override;
  std::shared_ptr<model::Module> editorAddedModulator2(Model::Type code) override;
  std::shared_ptr<Module> getModulator(int index) override;
  std::shared_ptr<model::Module> getModulator2(int index) override;
  std::shared_ptr<Module> editorAddedModulator(Model::Type code) override;
  Preset editorChangedPreset(int index) override;
  void clear();
  Preset getStateRepresentation() override;
  juce::Array<std::shared_ptr<Module>> getModulators() override;
  std::vector<std::shared_ptr<model::Module>> getModulators2() override;
  juce::Array<std::shared_ptr<Modulation>> getConnectionsOfSource(std::shared_ptr<Module> source) override;
  // juce::Array<std::shared_ptr<model::Modulation>> getModulations() override;
  std::vector<std::shared_ptr<model::Connection>> getModulations() override;

  std::pair<float, float> editorRequestsModulatorValue(Index moduleIndex, int parameterIndex, int modulatorIndex) override;
  std::pair<float, float> editorRequestsModulatorValue(int modulationConnectionIndex) override;
  juce::StringArray editorRequestsPresetNames() override;
  juce::Array<juce::MPENote> editorRequestsCurrentlyPlayingNotes() override;
  const vital::StatusOutput* editorRequestsStatusOutput(std::string name) override;
  // *********************************************************
private:
  bool editor_ready_ = false;
  bool block_modified_ = false;
  bool engine_prepared_ = false;
  std::optional<Preset> pending_preset_;

  ValueBridge* bypass_parameter_;
  double last_seconds_time_;
  MidiKeyboardState keyboard_state_;
  MainComponent* main_component_;
  PresetManager preset_manager_;
  void loadPreset(Preset preset);
  void setValue(std::string module_id, std::string parameter, float value);
  std::string getStateString();

  juce::AudioPlayHead::CurrentPositionInfo position_info_;

  std::map<std::string, ValueBridge*> bridge_lookup_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};