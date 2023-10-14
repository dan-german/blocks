/*
  ==============================================================================

    ModulationInput.cpp
    Created: 18 Apr 2021 6:55:09am
    Author:  Dan German

  ==============================================================================
*/

#include "ModulationInput.h"
#include "Processor.h"

ModulationInput::ModulationInput(std::shared_ptr<Processor> modulator, std::shared_ptr<Modulation> connection): source(modulator), connection(connection) {}
ModulationInput::~ModulationInput() { }
bool ModulationInput::isActive() { return source->isActive(); }

float ModulationInput::getValue(int phase) {
  float value = source->getValue(phase);
  float mapped = jmap(value, -1.0f, 1.0f, 0.0f, 1.0f) - connection->getBipolar() * 0.5f;
  return mapped * connection->getMagnitude();
}