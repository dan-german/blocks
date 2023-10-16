/*
  ==============================================================================

  This file was auto-generated!

  It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "settings/WaveTableConstants.h"
#include "util/Analytics.h"
#include "settings/UserSettings.h"

PluginProcessor::~PluginProcessor() {
  Analytics::shared()->handleQuit();
}

PluginProcessor::PluginProcessor(): AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)) { setup(); }
PluginProcessor::PluginProcessor(bool): AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true).withInput("Input", AudioChannelSet::stereo(), true)) { setup(); }

void PluginProcessor::setup() {
  loadParameters();
  keyboardState = nullptr;
  WaveTableConstants::load(44100);
  Analytics::shared()->initProfileIfNeeded();
  Analytics::shared()->handleLaunch(getWrapperTypeDescription(wrapperType));
}

void PluginProcessor::loadParameters() {
  for (auto module : synth.moduleManager.pool.allModules) {
    for (auto parameter : module->parameters) {
      addParameter(parameter->audioParameter);
      parameter->audioParameter->addListener(this);
    }
  }
}

void PluginProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages) {
  updateHostInfo(buffer.getNumSamples());

  if (keyboardState != nullptr && mainComponent != nullptr) {
    keyboardState->processNextMidiBuffer(midiMessages, 0, buffer.getNumSamples(), true);
  }

  originalSize = buffer.getNumSamples();
  bufferSize = std::min(originalSize, 128);

  buffer.clear();

  for (int i = 0; i < originalSize; i += bufferSize) {
    for (auto voice : synth.blockVoices) {
      voice->bpm = bpm;
    }

    int current_samples = std::min<int>(bufferSize, originalSize - i);
    synth.renderNextBlock(buffer, midiMessages, i, current_samples);
  }
}

inline void PluginProcessor::updateHostInfo(int blockSize) {
  if (JUCEApplication::isStandaloneApp()) return;

  if (const auto position = getPlayHead()->getPosition()) {
    if (const auto bpm = position->getBpm()) {
      this->bpm = *bpm;
      synth.bpm = *bpm;
    }

    if (position->getIsPlaying() || position->getIsLooping() || position->getIsRecording()) {
      synth.correctToTime(*position->getTimeInSamples());
    }
  }
}

void PluginProcessor::prepareToPlay(double sampleRate, int samplesPerBlock) {
  synth.initialize(sampleRate, samplesPerBlock);
  isPrepared = true;
}

#ifndef JucePlugin_PreferredChannelConfigurations

bool PluginProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const {
#if JucePlugin_IsMidiEffect
  ignoreUnused(layouts);
  return true;
#else
  // This is the place where you check if the layout is supported.
  // In this template code we only support mono or stereo.
  if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
    && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo()) {
    return false;
  }

  // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
  if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
    return false;
#endif

  return true;
#endif
}
#endif

void PluginProcessor::getStateInformation(MemoryBlock& destData) {
  std::string state = synth.getState();
  destData.replaceAll(state.data(), state.size());
}

void PluginProcessor::setStateInformation(const void* data, int sizeInBytes) {
  auto presetString = String::fromUTF8(static_cast<const char*> (data), sizeInBytes);
  if (auto preset = synth.presetManager.stringToPreset(presetString.toStdString())) {
    if (isPrepared) {
      synth.loadPreset(*preset);
      if (editorReady) mainComponent->loadState(*preset);
    } else {
      synth.presetToLoadOnInit = *preset;
    }
  }
}

void PluginProcessor::parameterValueChanged(int parameterIndex, float newValue) {
  getParameters()[parameterIndex]->setValue(newValue);
}

AudioProcessorEditor* PluginProcessor::createEditor() {
  auto editor = new PluginEditor(*this);
  keyboardState = &editor->mainComponent.uiLayer.keyboardState;
  if (TopLevelWindow::getNumTopLevelWindows() == 1) {
    TopLevelWindow* w = TopLevelWindow::getTopLevelWindow(0);
    w->setUsingNativeTitleBar(true);
  }
  mainComponent = &editor->mainComponent;
  editorReady = true;
  return editor;
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter() {
  return new PluginProcessor();
}
