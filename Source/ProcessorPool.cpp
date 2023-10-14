/*
  ==============================================================================

    ProcessorPool.cpp
    Created: 22 Jul 2021 2:49:52pm
    Author:  Dan German

  ==============================================================================
*/

#include "ProcessorPool.h"

ProcessorPool::~ProcessorPool() { }

ProcessorPool::ProcessorPool() {
  int maxUniqueProcessorsPerVoice = 20;
  int processorCount = maxUniqueProcessorsPerVoice;

  for (int i = 0; i < Model::Types::all.size(); i++) {
    auto code = Model::Types::all[i];
    for (int j = 0; j < processorCount; j++) {
      processors[code].add(ModuleProcessorFactory::createProcessor(code));
    }
  }
}

std::shared_ptr<Processor> ProcessorPool::getProcessor(std::shared_ptr<Module> module) {
  auto processor = processors[module->id.type].removeAndReturn(0);

  if (processor) {
    processor->setModule(module);
    return processor;
  }

  return nullptr;
}