/*
  ==============================================================================

    GraphManager.cpp
    Created: 25 May 2021 1:53:19am
    Author:  Dan German

  ==============================================================================
*/

#include "dsp/GraphManager.h"
#include "dsp/Processor.h"
#include "dsp/NodeProcessor.h"
#include "settings/Constants.h"

GraphManager::GraphManager(int sampleRate, int bufferSize): graph(new AudioProcessorGraph()) {
  // graph->setPlayConfigDetails(2, 2, sampleRate, bufferSize);
  // graph->prepareToPlay(sampleRate, bufferSize);
  // outputNode = graph->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));

  // for (int r = 0; r < Constants::rows; r++)
  //   for (int c = 0; c < Constants::columns; c++)
  //     processorMatrix[r][c] = nullptr;

  // int rows = 7;
  // int columns = 5;

  // processor_matrix_.resize(Constants::rows);
  // for (int i = 0; i < Constants::columns; i++) {
  //   processor_matrix_[i].resize(Constants::columns);
  // }
}

GraphManager::~GraphManager() {
  const MessageManagerLock messageManagerLock;

  for (int r = 0; r < Constants::rows; r++) {
    for (int c = 0; c < Constants::columns; c++) {
      nodeMatrix[r][c].reset();
    }
  }
}

void GraphManager::addNode(std::shared_ptr<Processor> processor, Index index) {
  processorMatrix[index.row][index.column] = processor;
  processors.push_back(processor);
  processor->prepareToPlay(graph->getSampleRate(), graph->getBlockSize());
  nodeMatrix[index.row][index.column] = graph->addNode(std::make_unique<NodeProcessor>(processor.get()));
}

void GraphManager::connectTwoNodes(const Node::Ptr& row, const Node::Ptr& column) {
  graph->addConnection({ { row->nodeID, 0}, {column->nodeID, 0} });
  graph->addConnection({ { row->nodeID, 1}, {column->nodeID, 1} });
}

void GraphManager::processBlock(AudioBuffer<float>& audioBuffer, MidiBuffer& midiBuffer) {
  graph->processBlock(audioBuffer, midiBuffer);
}

void GraphManager::repositionProcessor(Index oldIndex, Index newIndex) {
  processorMatrix[newIndex.row][newIndex.column] = processorMatrix[oldIndex.row][oldIndex.column];
  processorMatrix[oldIndex.row][oldIndex.column] = nullptr;
  nodeMatrix[newIndex.row][newIndex.column] = nodeMatrix[oldIndex.row][oldIndex.column];
  nodeMatrix[oldIndex.row][oldIndex.column] = nullptr;

  reconnect();
}

std::shared_ptr<Processor> GraphManager::getModuleProcessor(Index index) {
  return processorMatrix[index.row][index.column];
}

void GraphManager::removeNode(std::shared_ptr<Processor> moduleProcessor, Index index) {
  if (auto node = nodeMatrix[index.row][index.column]) {
    graph->removeNode(node->nodeID);
    nodeMatrix[index.row][index.column].reset();
    nodeMatrix[index.row][index.column] = nullptr;

    if (auto nodeAbove = findNodeAbove(index))
      connectTwoNodes(nodeAbove, findNodeBelow(index));
  }

  processorMatrix[index.row][index.column] = nullptr;
  processors.erase(remove(processors.begin(), processors.end(), moduleProcessor), processors.end());
}

Node::Ptr GraphManager::findNodeAbove(Index index) {
  for (int r = index.row - 1; r >= 0; r--)
    if (auto nodeAbove = nodeMatrix[r][index.column]) {
      return nodeAbove;
    }

  return nullptr;
}

Node::Ptr GraphManager::findNodeBelow(Index index) {
  for (int r = index.row + 1; r < Constants::rows; r++) {
    if (auto nodeBelow = nodeMatrix[r][index.column]) {
      return nodeBelow;
    }
  }

  return outputNode;
}

std::vector<AudioProcessorGraph::Connection> GraphManager::reconnect() {
  disconnectAll();

  for (int r = 0; r < Constants::rows; r++)
    for (int c = 0; c < Constants::columns; c++) {
      if (auto nodeId = nodeMatrix[r][c]) {
        auto node = graph->getNodeForId(nodeId->nodeID);

        auto index = Index(r, c);

        auto nodeBelow = findNodeBelow(index);
        connectTwoNodes(node, nodeBelow);

        if (auto nodeAbove = findNodeAbove(index)) {
          graph->removeConnection({ {nodeAbove->nodeID, 0}, {nodeBelow->nodeID, 0} });
          graph->removeConnection({ {nodeAbove->nodeID, 1}, {nodeBelow->nodeID, 1} });

          connectTwoNodes(nodeAbove, node);
        }
      }
    }

  return graph->getConnections();
}

void GraphManager::disconnectAll() {
  // for (int column = 0; column < processor_matrix_.size(); column++) {
  //   for (int row = 0; row < processor_matrix_[column].size(); row++) {
  //     auto processor = processor_matrix_[column][row];
  //     if (processor != nullptr) {
  //       voice_sum_->unplug(processor_matrix_[column][row].get());
  //     }
  //   }
  // }
}

void GraphManager::clear() {
  for (int r = 0; r < Constants::rows; r++)
    for (int c = 0; c < Constants::columns; c++) {
      if (auto node = nodeMatrix[r][c]) {
        graph->removeNode(nodeMatrix[r][c]->nodeID);
        nodeMatrix[r][c].reset();
        nodeMatrix[r][c] = nullptr;
      }
      processorMatrix[r][c] = nullptr;
    }

  processors.clear();
}