#include "Synth.h"
#include "util/Analytics.h"

void Synth::editorAdjustedModulator(int parameter, int modulator, float value) { moduleManager.getModulator(modulator)->parameter(parameter)->setValue(value); }
void Synth::editorAdjustedBlock(Index index, int parameter, float value) { moduleManager.getBlock(index)->parameter(parameter)->setValue(value); }
void Synth::editorChangedModulationMagnitude(int index, float magnitude) { moduleManager.getConnection(index)->setMagnitude(magnitude); }
void Synth::editorChangedModulationPolarity(int index, bool bipolar) { moduleManager.getConnection(index)->setPolarity(bipolar); }
void Synth::editorAdjustedTab(int column, int parameter, float value) { moduleManager.getTab(column)->parameter(parameter)->setValue(value); }

std::shared_ptr<Module> Synth::getModulator(int index) { return moduleManager.getModulator(index); }

Array<std::shared_ptr<Modulation>> Synth::getConnectionsOfSource(std::shared_ptr<Module> source) { return moduleManager.getConnectionsOfSource(source); }
Array<std::shared_ptr<Modulation>> Synth::getModulations() { return moduleManager.getConnections(); }
std::shared_ptr<Block> Synth::getBlock(Index index) { return (index.row == -1 || index.column == -1) ? nullptr : moduleManager.getBlock(index); }
std::shared_ptr<Tab> Synth::getTab(int column) { return moduleManager.getTab(column); }
Array<MPENote> Synth::editorRequestsCurrentlyPlayingNotes() { return currentlyPlayingNotes; }
Array<int> Synth::editorRequestsActiveColumns() { return moduleManager.getActiveColumns(); }

void Synth::editorRepositionedBlock(Index oldIndex, Index newIndex) {
  Analytics::shared()->countAction("Block Repositioned");
  repositionProcessor(oldIndex, newIndex);
}

void Synth::editorConnectedModulation(int modulatorIndex, std::string targetName, std::string parameter) {

  Analytics::shared()->countAction("Modulation Connected");
  // connect(modulatorIndex, targetName, parameter);
}

void Synth::editorDisconnectedModulation(int index) {
  Analytics::shared()->countAction("Modulation Disconnected");
  disconnect(index);
}

PresetInfo Synth::editorChangedPreset(int index) {
  return changePreset(index);
}

void Synth::editorRemovedTab(int column) {
  Analytics::shared()->countAction("Tab Removed");
  removeTab(column);
}

std::shared_ptr<Tab> Synth::editorAddedTab(int column) {
  auto type = Model::Types::noteTab;
  Analytics::shared()->countAction(type + " Tab Added");
  return moduleManager.addTab(type, column, -1);
}

void Synth::editorRemovedModulator(int index) {
  Analytics::shared()->countAction("Modulator Removed");
  removeModulator(index);
}

std::shared_ptr<Module> Synth::editorAddedModulator(Model::Type code) {
  Analytics::shared()->countAction("Modulator Added");
  return addModulator(code);
}

void Synth::editorRemovedBlock(Index index) {
  Analytics::shared()->countAction("Block Removed");
  removeBlock(index);
}

std::shared_ptr<Block> Synth::editorAddedBlock(Model::Type type, Index index) {
  Analytics::shared()->countAction(type + " Block Added");
  return addBlock(type, index, -1);
}

void Synth::editorRepositionedTab(int oldColumn, int newColumn) {
  Analytics::shared()->countAction("Tab Repositioned");

  auto tab = moduleManager.getTab(oldColumn);
  auto columns = tab->getAllColumns();
  auto currentBlockVoice = blockVoices[getCurrentVoiceIndex()];
  currentBlockVoice->setColumnsState(columns, false);

  tab->column = newColumn;
}

void Synth::editorChangedTabLength(int column, int length) {
  Analytics::shared()->countAction("Tab Length Changed");
  auto tab = moduleManager.getTab(column);
  tab->length = length;
}

void Synth::editorChangedBlockLength(Index index, int length) {
  Analytics::shared()->countAction("Block Length Changed");
  auto parent = moduleManager.getBlock(index);
  auto difference = length - parent->length;
  bool expanded = difference > 0;

  if (expanded) {
    expand(index, difference, true);
  } else {
    reduce(index, abs(difference));
  }
}

Array<std::shared_ptr<Module>> Synth::getModulators() {
  Array<std::shared_ptr<Module>> array;
  for (auto modulator : moduleManager.modulators) array.add(modulator);
  return array;
}

void Synth::editorSavedPreset(String name) {
  Analytics::shared()->countAction("Preset Saved");
  auto info = PresetInfo::create(name, moduleManager.getTabs(), moduleManager.getBlocks(), moduleManager.getModulators(), moduleManager.getConnections());
  presetManager.save(info);
}

std::pair<float, float> Synth::editorRequestsModulatorValue(Index moduleIndex, int parameterIndex, int modulatorIndex) {
  auto voice = blockVoices[getCurrentVoiceIndex()];
  auto processor = voice->getProcessor(moduleIndex);
  auto parameter = processor->getParameter(parameterIndex);
  auto modulationInput = parameter->modulationInputs[modulatorIndex];

  if (modulationInput)
    return { modulationInput->source->getValue(0), modulationInput->connection->getMagnitude() };

  return { 0.0f, 0.0f };
}

std::pair<float, float> Synth::editorRequestsModulatorValue(int modulationConnectionIndex) {
  auto connection = moduleManager.getConnection(modulationConnectionIndex);
  auto currentVoice = blockVoices[getCurrentVoiceIndex()];

  if (auto modulationInput = currentVoice->getModulationInput(connection))
    return { modulationInput->source->getValue(0), modulationInput->connection->getMagnitude() };

  return { 0.0f, 0.0f };
}

PresetInfo Synth::getStateRepresentation() {
  auto currentState = PresetInfo();

  for (auto block : moduleManager.getBlocks()) {
    auto presetBlock = PresetInfo::Block();
    presetBlock.index = { block->index.row, block->index.column }; ;
    presetBlock.id = block->id;
    presetBlock.length = block->length;
    currentState.blocks.add(presetBlock);
  }

  for (auto tab : moduleManager.getTabs()) {
    auto presetTab = PresetInfo::Tab();
    presetTab.column = tab->column;
    presetTab.id = tab->id;
    currentState.tabs.add(presetTab);
  }

  return currentState;
}

MPESynthesiserVoice* Synth::findFreeVoice(MPENote noteToFindVoiceFor, bool stealIfNoneAvailable) const {
  auto voice = MPESynthesiser::findFreeVoice(noteToFindVoiceFor, stealIfNoneAvailable);
  currentVoice = voice;
  return voice;
}

int Synth::getCurrentVoiceIndex() {
  auto index = voices.indexOf(currentVoice);
  return index == -1 ? 0 : index;
}

Synth::Synth() {
  enableLegacyMode(24);
  setVoiceStealingEnabled(true);
  setMinimumRenderingSubdivisionSize(128, false);

  delayManager.onTapStarted = [this](MPENote note) { this->handleDelayTapStarted(note); };
  delayManager.onTapEnded = [this](MPENote note) { this->handleDelayTapEnded(note); };
}

void Synth::handleDelayTapStarted(MPENote note) {
  // start the note only for delay triggered tabs

  // auto voice = findFreeVoice(note, true);
  // auto tabs = modules.tabManager.getTabs(Tab::Trigger::delay);

  // for (auto tab : tabs) {
    // auto mode = tab->getSelection();
    // if (mode == Tab::Selection::random)
    // if (tab->getse/han)
    // voice->s
  // }

  // instrument.noteOn(note.midiChannel, note.initialNote, note.noteOnVelocity);
}

void Synth::handleDelayTapEnded(MPENote note) {
  instrument.noteOff(note.midiChannel, note.initialNote, note.noteOffVelocity);
}

void Synth::initialize(double sampleRate, int bufferSize) {
  auto state = getState();

  this->sampleRate = sampleRate;
  this->bufferSize = bufferSize;
  setCurrentPlaybackSampleRate(sampleRate);
  spawnVoices(VOICE_COUNT);

  if (presetToLoadOnInit) {
    loadPreset(*presetToLoadOnInit);
    presetToLoadOnInit = std::nullopt;
  } else if (auto preset = presetManager.stringToPreset(state)) {
    loadPreset(*preset);
  }
}

void Synth::removeBlock(Index index) {
  auto block = moduleManager.getBlock(index);

  for (auto* voice : blockVoices)
    for (int i = 0; i < block->length; i++)
      voice->removeBlock(block->index.toTheRight(i));

  removeConnectionsFromTarget(block);
  moduleManager.removeBlock(block);
}

void Synth::removeTab(int column) {
  auto tab = moduleManager.getTab(column);
  removeConnectionsFromTarget(tab);
  moduleManager.removeTab(tab);
}

void Synth::spawnVoices(int count) {
  moduleManager.clear();
  blockVoices.clear();
  clearVoices();

  for (int i = 0; i < count; i++) {
    int minBufferSize = std::min(bufferSize, 128);
    auto voice = new Voice(sampleRate, minBufferSize);
    voice->listeners.add(this);
    voice->id = i;
    blockVoices.add(voice);
    addVoice(voice);
  }
}

void Synth::reroute(Index index) {
  GraphManager::Connections connections = blockVoices[0]->graphManager.getRouteForIndex(index);

  for (auto voice : blockVoices) {
    for (auto addition : connections.additions)
      voice->graphManager.graph->addConnection(addition);

    for (auto removal : connections.removals)
      voice->graphManager.graph->removeConnection(removal);
  }
}

void Synth::repositionProcessor(Index oldIndex, Index newIndex) {
  auto block = getBlock(oldIndex);

  if (block->length > 1) {
    repositionGroup(oldIndex, newIndex);
  } else {
    for (auto voice : blockVoices) {
      voice->repositionProcessor(oldIndex, newIndex);
    }
  }

  moduleManager.repositionBlock(oldIndex, newIndex);
}

void Synth::repositionGroup(const Index& oldIndex, const Index& newIndex) {
  auto block = moduleManager.getBlock(oldIndex);

  bool wentRight = oldIndex.column < newIndex.column;
  if (wentRight) {
    for (int newColumn = block->length - 1; newColumn >= 0; newColumn--) {
      auto currentOldIndex = Index(oldIndex.row, oldIndex.column + newColumn);
      auto currentNewIndex = Index(newIndex.row, newIndex.column + newColumn);

      for (auto voice : blockVoices) {
        voice->repositionProcessor(currentOldIndex, currentNewIndex);
      }
    }
  } else {
    for (int newColumn = 0; newColumn < block->length; newColumn++) {
      auto currentOldIndex = Index(oldIndex.row, oldIndex.column + newColumn);
      auto currentNewIndex = Index(newIndex.row, newIndex.column + newColumn);

      for (auto voice : blockVoices) {
        voice->repositionProcessor(currentOldIndex, currentNewIndex);
      }
    }
  }
}

void Synth::disconnect(std::shared_ptr<Modulation> connection) {
  for (auto voice : blockVoices)
    voice->disconnect(connection);

  moduleManager.removeConnection(connection);
}

std::shared_ptr<Modulation> Synth::connect(int modulatorIndex, String targetName, int parameterIndex, int number) {
  auto source = moduleManager.getModulator(modulatorIndex);
  auto target = moduleManager.getModule(targetName);

  if (auto connection = moduleManager.addConnection(source, target, parameterIndex, number)) {
    for (auto voice : blockVoices)
      voice->connect(connection);

    return connection;
  }

  return nullptr;
}

void Synth::removeModulator(int index) {
  for (auto connection : moduleManager.getConnectionsOfSource(getModulator(index)))
    disconnect(connection);

  moduleManager.removeModulator(index);

  for (auto voice : blockVoices) {
    voice->removeModulator(index);
  }
}

void Synth::addModulator(std::shared_ptr<Module> module) {
  for (auto voice : blockVoices) {
    voice->addModulator(module);
  }
}

void Synth::reduce(Index index, int times) {
  auto parent = moduleManager.getBlock(index);
  Array<Index> indexes;

  for (auto voice : blockVoices)
    for (int i = parent->length; i > parent->length - times; i--) {
      auto childIndex = parent->index.toTheRight(i - 1);
      voice->removeBlock(childIndex);
    }

  for (auto i : indexes) reroute(i);
  parent->length -= times;
}

void Synth::expand(Index index, int times, bool updateLength) {
  auto parent = moduleManager.getBlock(index);
  Array<Index> indexes;

  for (int i = 0; i < times; i++) {
    auto indexToCopyTo = parent->index.toTheRight(parent->length + i);
    indexes.add(indexToCopyTo);
    for (auto voice : blockVoices) {
      voice->addChild(parent, indexToCopyTo);
    }
  }

  for (auto i : indexes) reroute(i);
  if (updateLength) parent->length += times;
}

void Synth::clear() {
  moduleManager.clear();

  for (auto voice : blockVoices) {
    voice->clear();
    voice->listeners.add(this);
  }
}

void Synth::editorParameterGestureChanged(String moduleName, int parameterIndex, bool started) {
  if (JUCE_STANDALONE_APPLICATION) return;

  if (started) {
    moduleManager.getModule(moduleName)->parameter(parameterIndex)->audioParameter->beginChangeGesture();
  } else {
    moduleManager.getModule(moduleName)->parameter(parameterIndex)->audioParameter->endChangeGesture();
  }
}

StringArray Synth::editorRequestsPresetNames() {
  StringArray result;

  for (auto preset : presetManager.presets)
    result.add(preset.name);

  return result;
}

void Synth::disconnect(int index) {
  std::shared_ptr<Modulation> connection = moduleManager.getConnection(index);

  for (auto voice : blockVoices)
    voice->disconnect(connection);

  moduleManager.removeConnection(index);
}

std::shared_ptr<Block> Synth::addBlock(Model::Type type, Index index, int number) {
  auto block = moduleManager.addBlock(type, index, number);

  if (block == nullptr) return nullptr;

  for (auto voice : blockVoices) {
    voice->addBlock(block);
  }

  reroute(block->index);
  return block;
}

std::shared_ptr<Module> Synth::addModulator(Model::Type type, int number, int colourId) {
  auto modulator = moduleManager.addModulator(type, number, colourId);

  if (modulator == nullptr) {
    return nullptr;
  }

  addModulator(modulator);
  return modulator;
}

void Synth::removeConnectionsFromTarget(std::shared_ptr<Module> module) {
  for (auto connection : moduleManager.getConnectionsOfTarget(module))
    disconnect(connection);
}

PresetInfo Synth::changePreset(int index) {
  if (index == -1) {
    Analytics::shared()->countAction("Preset Initialized");
    clear();
    return PresetInfo();
  }

  Analytics::shared()->countAction("Preset Changed");
  auto preset = presetManager.presets[index];
  loadPreset(preset);
  return preset;
}

void Synth::loadPreset(PresetInfo preset) {
  clear();

  for (auto presetTab : preset.tabs) {
    auto tab = moduleManager.addTab(presetTab.id.type, presetTab.column, presetTab.id.number);
    tab->length = presetTab.length;

    for (auto const& [key, val] : presetTab.parameters)
      tab->parameterMap[key]->audioParameter->setValue(val);
  }

  for (auto presetBlock : preset.blocks) {
    Index index = { presetBlock.index.first, presetBlock.index.second };
    auto block = addBlock(presetBlock.id.type, index, presetBlock.id.number);

    for (auto const& [key, val] : presetBlock.parameters) {
      block->parameterMap[key]->audioParameter->setValue(val);
    }

    if (presetBlock.length > 1) {
      expand(block->index, presetBlock.length - 1, true);
    }
  }

  for (auto presetModulator : preset.modulators) {
    auto modulator = addModulator(presetModulator.id.type, presetModulator.id.number, presetModulator.colour);
    for (auto const& [key, val] : presetModulator.parameters)
      modulator->parameterMap[key]->audioParameter->setValue(val);
  }

  for (auto presetConnection : preset.modulations) {
    auto modulator = moduleManager.getModule(presetConnection.source);

    int modulatorIndex = moduleManager.modulators.indexOf(modulator);

    auto target = moduleManager.getModule(presetConnection.target);
    auto parameterName = presetConnection.parameter;
    auto parameter = target->parameterMap[parameterName];
    auto parameterIndex = target->parameters.indexOf(parameter);
    auto modulation = connect(modulatorIndex, presetConnection.target, parameterIndex, presetConnection.number);

    modulation->magnitudeParameter->setValue(presetConnection.magnitude);
    modulation->setPolarity(presetConnection.bipolar);
  }
}

void Synth::noteStopped(bool allowTailOff) {}

std::string Synth::getState() {
  auto info = PresetInfo::create(name, moduleManager.getTabs(), moduleManager.getBlocks(), moduleManager.getModulators(), moduleManager.getConnections());
  return presetManager.presetToString(info);
}

void Synth::noteStarted(Voice* voice, float frequencyInHertz) {
  moduleManager.triggerNoteInTabs(voice);

  currentlyPlayingNotes.add(voice->getCurrentlyPlayingNote());
}

void Synth::renderNextSubBlock(AudioBuffer<float>& outputAudio, int startSample, int numSamples) {
  MPESynthesiser::renderNextSubBlock(outputAudio, startSample, numSamples);
  delayManager.process(numSamples);
};

void Synth::noteReleased(MPENote finishedNote) {
  int id = stopVoiceByNote(finishedNote);

  currentlyPlayingNotes.removeIf([finishedNote](MPENote note) { return note.noteID == finishedNote.noteID; });
}

void Synth::triggerReleasePylons(MPENote& finishedNote, int id) { }

int Synth::stopVoiceByNote(MPENote note) {
  for (auto i = voices.size(); --i >= 0;) {
    auto* voice = blockVoices.getUnchecked(i);

    if (voice->note == note) {
      stopVoice(voice, note, true);
      return voice->id;
    }
  }
  return -1;
}

void Synth::handleMidiEvent(const MidiMessage& message) {
  MPESynthesiser::handleMidiEvent(message);

  if (message.isNoteOn()) {
    MPENote note;
    note.midiChannel = message.getChannel();
    note.initialNote = message.getNoteNumber();
    note.noteOnVelocity = MPEValue::from7BitInt(message.getVelocity());
  }

  if (message.isNoteOff()) {
    MPENote note;
    note.midiChannel = message.getChannel();
    note.initialNote = message.getNoteNumber();
    note.noteOnVelocity = MPEValue::from7BitInt(message.getVelocity());
  }
}

// copied the code from the base function, but removed some parts. 
MPESynthesiserVoice* Synth::findVoiceToSteal(MPENote noteToStealVoiceFor) const {
  jassert(voices.size() > 0);

  MPESynthesiserVoice* low = nullptr; // Lowest sounding note, might be sustained, but NOT in release phase
  MPESynthesiserVoice* top = nullptr; // Highest sounding note, might be sustained, but NOT in release phase

  Array<MPESynthesiserVoice*> usableVoices;
  usableVoices.ensureStorageAllocated(voices.size());

  for (auto* voice : voices) {
    jassert(voice->isActive()); // We wouldn't be here otherwise

    usableVoices.add(voice);
    struct Sorter
    {
      bool operator() (const MPESynthesiserVoice* a, const MPESynthesiserVoice* b) const noexcept { return a->noteOnTime < b->noteOnTime; }
    };

    std::sort(usableVoices.begin(), usableVoices.end(), Sorter());
  }

  if (top == low) top = nullptr;

  for (auto* voice : usableVoices)
    if (voice != low && voice != top && voice->isPlayingButReleased())
      return voice;

  for (auto* voice : usableVoices)
    if (voice != low && voice != top
      && voice->getCurrentlyPlayingNote().keyState != MPENote::keyDown
      && voice->getCurrentlyPlayingNote().keyState != MPENote::keyDownAndSustained) {
      return voice;
    }

  for (auto* voice : usableVoices)
    if (voice != low && voice != top)
      return voice;

  jassert(low != nullptr);

  return top != nullptr ? top : low;
  if (top != nullptr) return top;

  return low;
}

void Synth::correctToTime(int64 timeInSamples) {
  for (auto voice : blockVoices) {
    voice->correctToTime(timeInSamples);
  }
}