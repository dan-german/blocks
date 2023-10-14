/*
  ==============================================================================

    GraphManager.cpp
    Created: 25 May 2021 1:53:19am
    Author:  Dan German

  ==============================================================================
*/

#include "GraphManager.h"
#include "Processor.h"
#include "NodeProcessor.h"

GraphManager::GraphManager(int sampleRate, int bufferSize): graph(new AudioProcessorGraph()) {
  graph->setPlayConfigDetails(2, 2, sampleRate, bufferSize);
  graph->prepareToPlay(sampleRate, bufferSize);
  outputNode = graph->addNode(std::make_unique<AudioGraphIOProcessor>(AudioGraphIOProcessor::audioOutputNode));

  for (int r = 0; r < Constants::rows; r++)
    for (int c = 0; c < Constants::columns; c++)
      processorMatrix[r][c] = nullptr;
}

GraphManager::~GraphManager() {
  const MessageManagerLock messageManagerLock;

  for (int r = 0; r < Constants::rows; r++) {
    for (int c = 0; c < Constants::columns; c++) {
      nodeMatrix[r][c].reset();
    }
  }
}

GraphManager::Connections GraphManager::getRouteForIndex(Index index) {
  Connections connections;
  auto node = nodeMatrix[index.row][index.column];

  auto nodeBelow = findNodeBelow(index);

  connections.additions.push_back({ { node->nodeID, 0}, {nodeBelow->nodeID, 0} });
  connections.additions.push_back({ { node->nodeID, 1}, {nodeBelow->nodeID, 1} });

  if (auto nodeAbove = findNodeAbove(index)) {
    connections.removals.push_back({ { nodeAbove->nodeID, 0}, {nodeBelow->nodeID, 0} });
    connections.removals.push_back({ { nodeAbove->nodeID, 1}, {nodeBelow->nodeID, 1} });

    connections.additions.push_back({ { nodeAbove->nodeID, 0}, {node->nodeID, 0} });
    connections.additions.push_back({ { nodeAbove->nodeID, 1}, {node->nodeID, 1} });
  }

  return connections;
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
  for (int r = 0; r < Constants::rows; r++)
    for (int c = 0; c < Constants::columns; c++)
      if (auto nodeId = nodeMatrix[r][c])
        graph->disconnectNode(nodeId->nodeID);
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