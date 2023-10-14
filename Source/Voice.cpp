/*
  ==============================================================================

  Voice.cpp
  Created: 3 Oct 2020 11:00:19am
  Author:  dange

  ==============================================================================
*/

#include "Voice.h"
#include "OscillatorModule.h"
#include "ProcessorPool.h"

Voice::Voice(int sampleRate, int bufferSize): graphManager(sampleRate, bufferSize), voiceBuffer(2, bufferSize) {
  pool.reset(new ProcessorPool());
}

Voice::~Voice() { }

void Voice::notePressureChanged() {}
void Voice::notePitchbendChanged() {}
void Voice::noteTimbreChanged() {}
void Voice::noteKeyStateChanged() {}
void Voice::setLfoPhase(int64 seconds) { for (auto modulator : modulators) modulator->setPhase(seconds); }
void Voice::repositionProcessor(Index oldIndex, Index newIndex) { graphManager.repositionProcessor(oldIndex, newIndex); }

std::shared_ptr<Processor> Voice::getProcessor(String name) { return processorMap[name]; }
std::shared_ptr<Processor> Voice::getModulator(int index) { return modulators[index]; }
std::shared_ptr<Processor> Voice::getProcessor(Index index) { return graphManager.getModuleProcessor(index); }
std::shared_ptr<Processor> Voice::getModulator(String name) { return modulatorMap[name]; }

inline void Voice::processModulators(int samples) {
  for (auto modulator : modulators) {
    modulator->process(samples);
  }
}

void Voice::addModulator(std::shared_ptr<Module> module) {
  auto modulator = pool->getProcessor(module);
  modulator->prepareToPlay(getSampleRate(), graphManager.graph->getBlockSize());
  listeners.add(modulator.get());
  modulatorMap[module->name] = modulator;
  processorMap[module->name] = modulator;
  modulators.add(modulator);
}

std::shared_ptr<Processor> Voice::addBlock(std::shared_ptr<Block> block) {
  auto processor = pool->getProcessor(block);

  listeners.add(processor.get());
  graphManager.addNode(processor, block->index);

  if (block->isOscillator())
    oscillators.add(processor);

  processorMap[block->name] = processor;
  return processor;
}

void Voice::addChild(std::shared_ptr<Block> parent, Index index) {
  auto parentProcessor = processorMap[parent->name];
  auto newProcessor = pool->getProcessor(parent->id.type);

  newProcessor->parameters.clear();
  newProcessor->parameters.addArray(parentProcessor->parameters);
  newProcessor->module = parent;

  listeners.add(newProcessor.get());
  graphManager.addNode(newProcessor, index);
}

void Voice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
  for (auto processor : graphManager.processors) processor->bpm = bpm;

  processModulators(numSamples);

  voiceBuffer.clear();

  MidiBuffer incomingMidi;
  voiceBuffer.setSize(2, numSamples, false, false, true);
  graphManager.processBlock(voiceBuffer, incomingMidi);

  outputBuffer.addFrom(0, startSample, voiceBuffer.getReadPointer(0), numSamples);
  outputBuffer.addFrom(1, startSample, voiceBuffer.getReadPointer(1), numSamples);
}

void Voice::connect(std::shared_ptr<Modulation> connection) {
  auto sourceProcessor = modulatorMap[connection->source->name];
  auto targetProcessor = processorMap[connection->target->name];

  if (connection->isOscGainEnvelope()) {
    auto osc = static_cast<OscillatorProcessor*>(targetProcessor.get());
    auto envelopeModulator = static_cast<EnvelopeModulator*>(sourceProcessor.get());
    ADSR::Parameters* p = const_cast<ADSR::Parameters*>(&envelopeModulator->adsr.getParameters());
    osc->setADSRParameters(p);
    return;
  }

  targetProcessor->getParameter(connection->parameterIndex)->connectModulation(sourceProcessor, connection);
}

void Voice::disconnect(std::shared_ptr<Modulation> connection) {
  auto targetProcessor = processorMap[connection->target->name];

  if (connection->isOscGainEnvelope()) {
    auto osc = std::static_pointer_cast<OscillatorProcessor>(targetProcessor);
    osc.get()->setADSRParameters(nullptr);
    return;
  }

  targetProcessor->getParameter(connection->parameterIndex)->removeModulator(processorMap[connection->source->name]);
}

void Voice::removeBlock(Index index) {
  auto processor = graphManager.getModuleProcessor(index);
  listeners.removeFirstMatchingValue(processor.get());
  graphManager.removeNode(processor, index);
  pool->retire(processor);
}

std::shared_ptr<ModulationInput> Voice::getModulationInput(std::shared_ptr<Modulation> modulationConnection) {
  auto targetProcessor = processorMap[modulationConnection->target->name];
  auto sourceProcessor = modulatorMap[modulationConnection->source->name];

  for (auto modulationInput : targetProcessor->getParameter(modulationConnection->parameterIndex)->modulationInputs)
    if (modulationInput->source == sourceProcessor)
      return modulationInput;

  return nullptr;
}

std::shared_ptr<ModulationInput> Voice::getModulationInput(String source, String target, int parameterIndex) {
  for (auto modulationInput : processorMap[target]->getParameter(parameterIndex)->modulationInputs)
    if (modulationInput->source == processorMap[source])
      return modulationInput;

  return nullptr;
}

void Voice::clear() {
  for (auto modulator : modulators) {
    pool->retire(modulator);
  }

  for (auto processor : graphManager.processors) {
    pool->retire(processor);
  }

  listeners.clear();
  modulators.clear();
  graphManager.clear();
}

void Voice::removeModulator(int index) {
  auto modulator = modulators[index];
  pool->retire(modulator);
  listeners.removeFirstMatchingValue(modulator.get());
  modulators.remove(index);
}

void Voice::noteStopped(bool allowTailOff) {
  for (auto listener : listeners)
    listener->noteStopped(allowTailOff);

  note = {};
}

void Voice::noteStarted() {
  startNote();
}

void Voice::startNote(bool notifyListeners) {
  note = currentlyPlayingNote;
  float frequency = (float)currentlyPlayingNote.getFrequencyInHertz();

  if (!notifyListeners) return;
  for (auto listener : listeners)
    listener->noteStarted(this, frequency);
}

void Voice::correctToTime(int64 timeInSamples) {
  for (auto modulator : modulators) {
    modulator->setPhase(timeInSamples);
  }
}