/*
  ==============================================================================

  NodeProcessor.h
  Created: 1 Oct 2020 10:15:41pm
  Author:  dange

  ==============================================================================
*/

#pragma once

#include "EnvelopeModulator.h"
#include "Processor.h"

class NodeProcessor: public juce::AudioProcessor {
public:
  NodeProcessor(Processor* moduleProcessor);
  ~NodeProcessor();

  void prepareToPlay(double sampleRate, int bufferSize) override;
  void releaseResources() override;
  void processBlock(juce::AudioSampleBuffer&, juce::MidiBuffer&) override;

  void reset() override;

  juce::AudioProcessorEditor* createEditor() override;
  bool hasEditor() const override;

  const juce::String getName() const override;
  bool acceptsMidi() const override;
  bool producesMidi() const override;
  double getTailLengthSeconds() const override;

  int getNumPrograms() override;
  int getCurrentProgram() override;
  void setCurrentProgram(int) override;
  const juce::String getProgramName(int) override;
  void changeProgramName(int, const juce::String&) override;

  void getStateInformation(juce::MemoryBlock&) override;
  void setStateInformation(const void*, int) override;

  Processor* getModuleProcessor();
  void setModuleProcessor(Processor* moduleProcessor);
protected:
  Processor* moduleProcessor;
private:
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(NodeProcessor)
};
