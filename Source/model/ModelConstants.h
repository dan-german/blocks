/*
  ==============================================================================

    ModuleDefinitions.h
    Created: 22 Apr 2021 10:49:16am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/Module.h"

namespace model {
const StringArray waveforms { "sine", "was", "saw", "square", "triangle" };
const StringArray modulators { "lfo", "adsr" };
const StringArray effects { "filter", "drive", "reverb", "delay", "chorus", "flanger", "phaser", "comp" };
const StringArray tabs { "oscillator", "modulator", "effect" };

const StringArray block_popup_row_one { "osc", "noise", "filter", "drive", "flanger" };
const StringArray block_popup_row_two { "reverb", "delay", "chorus", "phaser" };

constexpr int MAX_MODULES_PER_TYPE = 5;
}