/*
  ==============================================================================

    ModuleProcessorFactory.h
    Created: 21 Apr 2021 11:24:49am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "Processor.h"
#include "Module.h"
#include "OscillatorProcessor.h"
#include "LFOModulator.h"
#include "ReverbProcessor.h"
#include "EnvelopeModulator.h"
#include "FilterProcessor.h"
#include "DelayProcessor.h"
#include "DriveProcessor.h"
#include "MixerProcessor.h"

class ModuleProcessorFactory {
public:
  static shared_ptr<Processor> createProcessor(Model::Type code) {
    if (code == Model::Types::osc)    return make_shared<OscillatorProcessor>();
    if (code == Model::Types::lfo)    return make_shared<LFOModulator>();
    if (code == Model::Types::reverb) return make_shared<ReverbProcessor>();
    if (code == Model::Types::adsr)   return make_shared<EnvelopeModulator>();
    if (code == Model::Types::filter) return make_shared<FilterProcessor>();
    if (code == Model::Types::delay)  return make_shared<DelayProcessor>();
    if (code == Model::Types::drive)  return make_shared<DriveProcessor>();
    if (code == Model::Types::mixer)  return make_shared<MixerProcessor>();
    return nullptr;
  };
};