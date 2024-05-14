/*
  ==============================================================================

    ModuleDefinitions.h
    Created: 22 Apr 2021 10:49:16am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

namespace model {
static const juce::StringArray waveforms { "sine", "was", "saw", "square", "triangle" };
static const juce::StringArray modulators { "lfo", "envelope", "random" };
static const juce::StringArray effects { "filter", "drive", "reverb", "delay", "chorus", "flanger", "phaser", "comp" };
static const juce::StringArray tabs { "oscillator", "modulator", "effect" };
static const int rows = 7;
static const int columns = 5;

static const juce::StringArray block_popup_column_one { "osc", "noise", "filter", "drive", "flanger" };
static const juce::StringArray block_popup_column_two { "reverb", "delay", "chorus", "phaser" };

constexpr int MAX_MODULES_PER_TYPE = 8;
}