#pragma once

#include "OscillatorProcessor.h"
#include "ReverbProcessor.h"
#include "Synth.h"
#include "MainComponent.h"
#include "MainComponent.h"

using namespace Model;

class PluginProcessor: public AudioProcessor, public AudioProcessorParameter::Listener {
public:
  MidiKeyboardState* keyboardState;
  Synth synth;
  double bpm = 120;

  int originalSize;
  int bufferSize;
  int channelCount;
  bool editorReady = false;

  PluginProcessor();
  /// Included a dummy parameter to have a different constructor
  PluginProcessor(bool);
  ~PluginProcessor();

  void prepareToPlay(double sampleRate, int samplesPerBlock) override;
  void releaseResources() override {};
  std::optional<PresetInfo> presetToLoadOnInit;
  bool isPrepared = false;

#ifndef JucePlugin_PreferredChannelConfigurations
  bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

  void processBlock(AudioBuffer<float>&, MidiBuffer&) override;
  AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override { return true; };
  const String getName() const override { return JucePlugin_Name; };
  bool acceptsMidi() const override { return true; };
  bool producesMidi() const override { return true; };
  bool isMidiEffect() const override { return false; }
  double getTailLengthSeconds() const override { return 0.0f; };
  int getNumPrograms() override { return 1; };
  int getCurrentProgram() override { return 0; };
  void setCurrentProgram(int index) override {};
  const String getProgramName(int index) override { return {}; };
  void changeProgramName(int index, const String& newName) override {};
  void getStateInformation(MemoryBlock& destData) override;
  void setStateInformation(const void* data, int sizeInBytes) override;
  void parameterValueChanged(int parameterIndex, float newValue) override;
  void parameterGestureChanged(int parameterIndex, bool gestureIsStarting) override {};
  AudioDeviceManager audioDeviceManager;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginProcessor)
    void loadParameters();
  void updateHostInfo(int blockSize);

private:
  MainComponent* mainComponent;
  void setup();
};
