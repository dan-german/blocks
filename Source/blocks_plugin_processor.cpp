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

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "vital/synthesis/synth_engine/sound_engine.h"
#include "vital/common/load_save.h"

PluginProcessor::PluginProcessor(): juce::AudioProcessor(BusesProperties().withOutput("Output", AudioChannelSet::stereo(), true)) {
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
