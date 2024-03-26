/*
  ==============================================================================

    OscillatorModule.h
    Created: 23 May 2021 6:36:38am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/Block.h"
#include "model/ModelConstants.h"
#include "model/ModuleParameter.h"

namespace Model {
struct OscillatorModule: public Block {
  enum Parameters { pWave, pTranspose, pTune, pUnison, pSpread, pGain, pPan };
  int waveIndex = 0;

  ~OscillatorModule() { }
  OscillatorModule(int waveIndex, int number): Block(Model::Types::osc, number) {
    category = Module::Category::source;
    this->waveIndex = waveIndex;
    createParameters();
  }

  void createParameters() {
    createChoiceParameter({ "wave", model::waveforms, waveIndex });

    std::function<String(double)> textFromValueFunction = [](double value) {
      double percentage = std::round(value * 100 * 100) / 100;
      std::ostringstream oss;
      oss << std::fixed << std::setprecision(2) << percentage;
      std::string percentageString = oss.str();
      return String(percentageString);
    };

    createIntParameter({ .name = "transpose", .min = -48, .max = 48, .defaultValue = 0 });
    createFloatParameter({ .name = "tune", .defaultValue = .0f, .textFromValueFunction = textFromValueFunction, .range = { -1.0f, 1.0f, 0.0001f } });
    createIntParameter({ .name = "unison", .min = 1, .max = 8, .defaultValue = 1 });
    createFloatParameter({ .name = "spread", .defaultValue = 0.005f, .textFromValueFunction = textFromValueFunction });
    createFloatParameter({ .name = "gain", .defaultValue = 0.5f, .textFromValueFunction = textFromValueFunction });
    createFloatParameter({ .name = "pan", .defaultValue = 0.0f, .textFromValueFunction = textFromValueFunction,  .range = { -1.0f, 1.0f, 0.0001f } });
  }
};
}
