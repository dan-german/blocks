/*
  ==============================================================================

    ProcessorPool.h
    Created: 22 Jul 2021 2:49:52pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "ModuleProcessorFactory.h"

class ProcessorPool {
private:
  std::map<Model::Type, Array<std::shared_ptr<Processor>>> processors;
public:
  ProcessorPool();
  ~ProcessorPool();

  std::shared_ptr<Processor> getProcessor(std::shared_ptr<Module> module);

  void retire(std::shared_ptr<Processor> processor) {
    processors[processor->module->id.type].add(processor);
  }

  void retire(std::shared_ptr<Processor> processor, Model::Type code) {
    processors[code].add(processor);
  }

  std::shared_ptr<Processor> getProcessor(Model::Type code) {
    return processors[code].isEmpty() ? nullptr : processors[code].removeAndReturn(0);
  }
};