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
  std::map<Model::Type, Array<shared_ptr<Processor>>> processors;
public:
  ProcessorPool();
  ~ProcessorPool();

  shared_ptr<Processor> getProcessor(shared_ptr<Module> module);

  void retire(shared_ptr<Processor> processor) {
    processors[processor->module->id.type].add(processor);
  }

  void retire(shared_ptr<Processor> processor, Model::Type code) {
    processors[code].add(processor);
  }

  shared_ptr<Processor> getProcessor(Model::Type code) {
    return processors[code].isEmpty() ? nullptr : processors[code].removeAndReturn(0);
  }
};