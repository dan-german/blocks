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

class ValueBridge;

class PluginProcessor: public SynthBase, public juce::AudioProcessor, public ValueBridge::Listener {
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

private:
  bool editorReady = false;

  ValueBridge* bypass_parameter_;
  double last_seconds_time_;
  MidiKeyboardState keyboard_state_;
  MainComponent* mainComponent;

  juce::AudioPlayHead::CurrentPositionInfo position_info_;

  std::map<std::string, ValueBridge*> bridge_lookup_;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
};