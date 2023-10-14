/*
  ==============================================================================

    GraphManager.h
    Created: 25 May 2021 1:53:19am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "Constants.h"
#include "Index.h"
#include <juce_audio_processors/juce_audio_processors.h>

using namespace juce;
using AudioGraphIOProcessor = AudioProcessorGraph::AudioGraphIOProcessor;
using Node = AudioProcessorGraph::Node;
using GraphConnection = AudioProcessorGraph::Connection;

class Processor;

class GraphManager {
public:
  struct Connections;

  std::vector<std::shared_ptr<Processor>> processors;

  GraphManager(int sampleRate, int bufferSize);
  ~GraphManager();
  std::vector<GraphConnection> reconnect();
  GraphManager::Connections getRouteForIndex(Index index);
  std::shared_ptr<Processor> getModuleProcessor(Index index);
  std::unique_ptr<AudioProcessorGraph> graph;
  void addNode(std::shared_ptr<Processor> processor, Index index);

  void processBlock(AudioBuffer<float>& audioBuffer, MidiBuffer& midiBuffer);
  void removeNode(std::shared_ptr<Processor> moduleProcessor, Index index);
  void repositionProcessor(Index oldIndex, Index newIndex);
  void clear();
  void disconnectAll();
  Node::Ptr getNode(int row, int column) { return nodeMatrix[row][column]; }

private:
  std::shared_ptr<Processor> processorMatrix[Constants::rows][Constants::columns];
  Node::Ptr nodeMatrix[Constants::rows][Constants::columns];
  Node::Ptr outputNode;
  Node::Ptr findNodeAbove(Index index);
  Node::Ptr findNodeBelow(Index index);
  void connectTwoNodes(const Node::Ptr& first, const Node::Ptr& second);
};

struct GraphManager::Connections {
  std::vector<GraphConnection> additions;
  std::vector<GraphConnection> removals;
};
