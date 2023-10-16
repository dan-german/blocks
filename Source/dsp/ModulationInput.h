/*
  ==============================================================================

    ModulationInput.h
    Created: 16 Apr 2021 5:34:11am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include <memory>
#include "model/Modulation.h"
#include "model/OscillatorModule.h"
#include <juce_core/juce_core.h>

class Processor;
using Modulation = Model::Modulation;

struct ModulationInput {
  std::shared_ptr<Modulation> connection;
  std::shared_ptr<Processor> source;

  ModulationInput(std::shared_ptr<Processor> source, std::shared_ptr<Modulation> connection);
  ~ModulationInput();

  float getValue(int phase);
  bool isActive();
  JUCE_LEAK_DETECTOR(ModulationInput);
};
