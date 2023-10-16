/*
  ==============================================================================

  Synthesizer.h
  Created: 3 Oct 2020 11:00:19am
  Author:  dange

  ==============================================================================
*/

#pragma once
#include "settings/Constants.h"
#include "dsp/GraphManager.h"
#include "model/Block.h"
#include "model/Modulation.h"
#include "dsp/ModulationInput.h"

using Module = Model::Module;
using Block = Model::Block;
using Modulation = Model::Modulation;

class EnvelopeModulator;
class ProcessorPool;

class Voice: public MPESynthesiserVoice {
public:
  GraphManager graphManager;
  double bpm = 120.0f;
  AudioBuffer<float> voiceBuffer;
  bool isReleaseVoice = false;
  bool isActive = false;
  int id = 0;
  std::optional<MPENote> note;
  HashMap<int, bool> columnsStates;

  struct Listener {
    virtual void noteStarted(Voice* voice, float frequencyInHertz) = 0;
    virtual void noteStopped(bool allowTailOff) = 0;
    virtual ~Listener() = default;
  };
  Array<Listener*> listeners;

  Voice(int sampleRate, int bufferSize);
  ~Voice() override;

  std::shared_ptr<Processor> addBlock(std::shared_ptr<Block> block);
  void addModulator(std::shared_ptr<Module> module);
  void repositionProcessor(Index oldIndex, Index newIndex);
  void startNote(bool notifyListeners = true);
  void connect(std::shared_ptr<Modulation> connection);
  void disconnect(std::shared_ptr<Modulation> connection);
  void setLfoPhase(int64 seconds);
  void addChild(std::shared_ptr<Block> parent, Index index);
  void clear();
  void correctToTime(int64 timeInSeconds);

  void setAll(bool enabled) {
    for (int row = 0; row < Constants::rows; row++)
      for (int column = 0; column < Constants::columns; column++)
        if (auto node = graphManager.getNode(row, column))
          node->setBypassed(!enabled);
  }

  void setColumnState(int column, bool enabled) {
    columnsStates.set(column, enabled);

    for (int row = 0; row < Constants::rows; row++)
      if (auto node = graphManager.getNode(row, column))
        node->setBypassed(!enabled);
  }

  void setColumnsState(Array<int> columns, bool enabled) {
    String s;

    for (auto column : columns) {
      s += String(column) + " ";
    }

    String boolString = enabled ? "true" : "false";

    for (auto column : columns) {
      setColumnState(column, enabled);
    }
  }

  Array<int> getActiveColumns() {
    Array<int> activeColumns;

    for (int column = 0; column < Constants::columns; column++) {
      String s = columnsStates[column] ? "true" : "false";
    }

    for (int column = 0; column < Constants::columns; column++)
      if (columnsStates[column])
        activeColumns.add(column);


    return activeColumns;
  }

  std::shared_ptr<ModulationInput> getModulationInput(std::shared_ptr<Modulation> modulationConnection);
  std::shared_ptr<ModulationInput> getModulationInput(String source, String target, int parameterIndex);

  void removeBlock(Index index);
  void removeModulator(int index);
  std::shared_ptr<Processor> getProcessor(Index index);
  std::shared_ptr<Processor> getModulator(int index);
  std::shared_ptr<Processor> getModulator(String name);
  std::shared_ptr<Processor> getProcessor(String name);

private:
  Array<std::shared_ptr<Processor>> modulators;
  Array<std::shared_ptr<Processor>> oscillators;
  std::map<String, std::shared_ptr<Processor>> modulatorMap;
  std::map<String, std::shared_ptr<Processor>> processorMap;

  std::unique_ptr<ProcessorPool> pool;

  void processModulators(int samples);
  // void clearInactiveVoices();
  void handleOscillatorGainModulation() {

  }
protected:
  // Inherited via MPESynthesiserVoice
  void noteStarted() override;
  void noteStopped(bool allowTailOff) override;
  void notePressureChanged() override;
  void notePitchbendChanged() override;
  void noteTimbreChanged() override;
  void noteKeyStateChanged() override;
  void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;
};