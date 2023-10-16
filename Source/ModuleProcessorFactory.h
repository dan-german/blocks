/*
  ==============================================================================

    ModuleProcessorFactory.h
    Created: 21 Apr 2021 11:24:49am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "dsp/Processor.h"
#include "model/Module.h"
#include "dsp/OscillatorProcessor.h"
#include "dsp/LFOModulator.h"
#include "dsp/ReverbProcessor.h"
#include "dsp/EnvelopeModulator.h"
#include "dsp/FilterProcessor.h"
#include "dsp/DelayProcessor.h"
#include "dsp/DriveProcessor.h"
#include "dsp/MixerProcessor.h"

class ModuleProcessorFactory {
public:
  static std::shared_ptr<Processor> createProcessor(Model::Type code) {
    if (code == Model::Types::osc)    return std::make_shared<OscillatorProcessor>();
    if (code == Model::Types::lfo)    return std::make_shared<LFOModulator>();
    if (code == Model::Types::reverb) return std::make_shared<ReverbProcessor>();
    if (code == Model::Types::adsr)   return std::make_shared<EnvelopeModulator>();
    if (code == Model::Types::filter) return std::make_shared<FilterProcessor>();
    if (code == Model::Types::delay)  return std::make_shared<DelayProcessor>();
    if (code == Model::Types::drive)  return std::make_shared<DriveProcessor>();
    if (code == Model::Types::mixer)  return std::make_shared<MixerProcessor>();
    return nullptr;
  };
};