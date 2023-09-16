/*
  ==============================================================================

    ModuleDefinitions.h
    Created: 22 Apr 2021 10:49:16am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "Module.h"

namespace Model {
  const StringArray waveforms { "saw", "sine", "square", "triangle", "noise" };
  const StringArray modulators { "lfo", "envelope" };
  const StringArray effects { "filter", "reverb", "delay", "drive", "mixer" };
  const StringArray tabs { "oscillator", "modulator", "effect" };
  constexpr int MAX_MODULES_PER_TYPE = 5;
}