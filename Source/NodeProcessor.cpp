#include "NodeProcessor.h"

NodeProcessor::NodeProcessor(Processor* moduleProcessor): moduleProcessor(moduleProcessor) {
  enableAllBuses();
}

void NodeProcessor::prepareToPlay(double sampleRate, int bufferSize) {
  setPlayConfigDetails(2, 2, sampleRate, bufferSize);
  getModuleProcessor()->prepareToPlay(sampleRate, bufferSize);
}

void NodeProcessor::processBlock(juce::AudioSampleBuffer& buffer, juce::MidiBuffer&) {
  getModuleProcessor()->process(buffer);
}

NodeProcessor::~NodeProcessor() { }
void NodeProcessor::releaseResources() {}
juce::AudioProcessorEditor* NodeProcessor::createEditor() { return nullptr; }
bool NodeProcessor::hasEditor() const { return false; }
const juce::String NodeProcessor::getName() const { return {}; }
bool NodeProcessor::acceptsMidi() const { return false; }
bool NodeProcessor::producesMidi() const { return false; }
double NodeProcessor::getTailLengthSeconds() const { return 0.0; }
int NodeProcessor::getNumPrograms() { return 0; }
int NodeProcessor::getCurrentProgram() { return 0; }
void NodeProcessor::setCurrentProgram(int) {}
const juce::String NodeProcessor::getProgramName(int) { return juce::String(); }
void NodeProcessor::changeProgramName(int, const juce::String&) {}
void NodeProcessor::getStateInformation(juce::MemoryBlock&) {}
void NodeProcessor::setStateInformation(const void*, int) {}
Processor* NodeProcessor::getModuleProcessor() { return moduleProcessor; }
void NodeProcessor::setModuleProcessor(Processor* moduleProcessor) { this->moduleProcessor = moduleProcessor; }
void NodeProcessor::reset() { AudioProcessor::reset(); }

