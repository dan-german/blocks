/*
 ==============================================================================

 ModuleFactory.h
 Created: 5 Jun 2021 12:37:55am
 Author:  Dan German

 ==============================================================================
 */

#pragma once

#include "model/ModuleFactory.h"
#include "model/OscillatorModule.h"
#include "model/ReverbModule.h"
#include "model/EnvelopeModule.h"
#include "model/FilterModule.h"
#include "model/LFOModule.h"
#include "model/DelayModule.h"
#include "model/DriveModule.h"  
#include "model/MixerModule.h"
#include "model/Module.h"
#include "gui/Tab.h"

using namespace Model;

class ModuleFactory {
public:
  static std::shared_ptr<Block> createBlock(Model::Type type, int number) {
    if (type == Types::osc) return std::make_shared<OscillatorModule>(0, number);
    if (type == Types::filter) return std::make_shared<FilterModule>(number);
    if (type == Types::reverb) return std::make_shared<ReverbModule>(number);
    if (type == Types::delay) return std::make_shared<DelayModule>(number);
    if (type == Types::drive) return std::make_shared<DriveModule>(number);
    if (type == Types::mixer) return std::make_shared<MixerModule>(number);
    jassert(false);
    return nullptr;
  }

  static std::shared_ptr<Module> createModulator(Model::Type type, int number) {
    if (type == Types::lfo) return std::make_shared<LFOModule>(number);
    if (type == Types::adsr) return std::make_shared<EnvelopeModule>(number);
    jassert(false);
    return nullptr;
  }

  static std::shared_ptr<Tab> createTab(Model::Type type, int number) { return std::make_shared<Tab>(type, number); }
};