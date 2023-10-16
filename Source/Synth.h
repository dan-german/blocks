/*
  ==============================================================================

    Synth.h
    Created: 27 Jun 2021 2:21:41pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "dsp/Voice.h"
#include "model/ProcessorPool.h"
#include "model/Modulation.h"
#include "MainComponent.h"
#include "model/ModuleManager.h"
#include "model/PresetManager.h"
#include "dsp/TabDelayManager.h"

using Modulation = Model::Modulation;
using Block = Model::Block;

#define VOICE_COUNT 16

class Synth: public MPESynthesiser, Voice::Listener, public MainComponent::Delegate {
private:
  String name, description, author;
  mutable MPESynthesiserVoice* currentVoice;
  double sampleRate;
  int bufferSize;
  void noteStarted(Voice* voice, float frequencyInHertz) override;
  void noteStopped(bool allowTailOff) override;

  Array<MPENote> currentlyPlayingNotes;

  TabDelayManager delayManager;

  int stopVoiceByNote(MPENote note);
protected:
  void renderNextSubBlock(AudioBuffer<float>& outputAudio, int startSample, int numSamples) override;
  void noteReleased(MPENote finishedNote) override;
  void handleMidiEvent(const MidiMessage&) override;
public:
  double bpm = 120.0f;
  double phase = 0;
  int64 hostPositionInSamples = 0;
  Array<Voice*> blockVoices;
  ModuleManager moduleManager;
  PresetManager presetManager;
  std::optional<PresetInfo> presetToLoadOnInit;

  Synth();

  void handleDelayTapStarted(MPENote note);
  void handleDelayTapEnded(MPENote note);

  MPESynthesiserVoice* findFreeVoice(MPENote noteToFindVoiceFor, bool stealIfNoneAvailable) const override;
  int getCurrentVoiceIndex();
  void initialize(double sampleRate, int bufferSize);
  void spawnVoices(int count);
  void removeBlock(Index block);
  void removeTab(int column);
  void repositionProcessor(Index oldIndex, Index newIndex);
  void disconnect(std::shared_ptr<Modulation> connection);
  void removeModulator(int index);
  void addModulator(std::shared_ptr<Module> module);
  void reduce(Index index, int times);
  void expand(Index index, int times, bool updateLength = false);
  void reroute(Index index);
  void clear();
  void repositionGroup(const Index& oldIndex, const Index& newIndex);
  std::shared_ptr<Modulation> connect(int modulatorIndex, String targetName, int parameterIndex, int number = -1);
  void disconnect(int index);
  std::shared_ptr<Block> addBlock(Model::Type type, Index index, int number = -1);
  std::shared_ptr<Module> addModulator(Model::Type tpye, int number = -1, int colourId = -1);
  void removeConnectionsFromTarget(std::shared_ptr<Module> module);
  PresetInfo changePreset(int index);
  void loadPreset(PresetInfo preset);
  std::string getState();
  void correctToTime(int64 timeInSamples);

  virtual MPESynthesiserVoice* findVoiceToSteal(MPENote noteToStealVoiceFor = MPENote()) const override;

  // MainComponent::Delegate
  std::shared_ptr<Tab> editorAddedTab(int column) override;
  void editorRepositionedTab(int oldColumn, int newColumn) override;
  void editorChangedTabLength(int column, int length) override;
  Array<int> editorRequestsActiveColumns() override;

  void editorRemovedTab(int column) override;
  void editorRemovedBlock(Index index) override;
  void editorRepositionedBlock(Index oldIndex, Index newIndex) override;
  void editorAdjustedBlock(Index index, int parameter, float value) override;
  void editorAdjustedTab(int column, int parameter, float value) override;
  void editorChangedModulationMagnitude(int connectionIndex, float magnitude) override;
  void editorParameterGestureChanged(String moduleName, int parameterIndex, bool started) override;
  void editorChangedModulationPolarity(int index, bool bipolar) override;
  void editorDisconnectedModulation(int index) override;
  void editorSavedPreset(String name) override;
  void editorConnectedModulation(int modulatorIndex, String targetName, int parameter) override;
  void editorChangedBlockLength(Index index, int length) override;
  void editorAdjustedModulator(int parameter, int modulator, float value) override;
  void editorRemovedModulator(int index) override;

  std::shared_ptr<Block> getBlock(Index index) override;
  std::shared_ptr<Tab> getTab(int column) override;
  std::shared_ptr<Block> editorAddedBlock(Model::Type code, Index index) override;
  std::shared_ptr<Module> getModulator(int index) override;
  std::shared_ptr<Module> editorAddedModulator(Model::Type code) override;
  PresetInfo editorChangedPreset(int index) override;
  PresetInfo getStateRepresentation() override;
  Array<std::shared_ptr<Module>> getModulators() override;
  Array<std::shared_ptr<Modulation>> getConnectionsOfSource(std::shared_ptr<Module> source) override;
  Array<std::shared_ptr<Modulation>> getModulations() override;

  std::pair<float, float> editorRequestsModulatorValue(Index moduleIndex, int parameterIndex, int modulatorIndex) override;
  std::pair<float, float> editorRequestsModulatorValue(int modulationConnectionIndex) override;
  StringArray editorRequestsPresetNames() override;
  Array<MPENote> editorRequestsCurrentlyPlayingNotes() override;
  // *********************************************************
  void triggerReleasePylons(MPENote& finishedNote, int id);
};
