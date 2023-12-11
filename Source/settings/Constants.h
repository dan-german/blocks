/*
 ==============================================================================

 ModuleComponentConstants.h
 Created: 15 Aug 2020 7:52:16pm
 Author:  dange

 ==============================================================================
 */

#pragma once
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

namespace Constants {
constexpr int moduleWidth = 68;
constexpr int moduleHeight = 35;
constexpr int gridDotSize = 4;
constexpr int rows = 7;
constexpr int columns = 5;
constexpr int gridEdgeSpacing = gridDotSize - 1;
constexpr int moduleSpacing = 2;
constexpr int tabHeight = 22;
constexpr float moduleGridSnapThreshold = 0.25f;
}

namespace Constants {
struct Functions {
  static float gridWidth() { return columns * moduleWidth + (columns - 1) * moduleSpacing + gridEdgeSpacing * 2; }
  static int tabWidth() { return int(gridWidth() + 1) / float(columns); }
};
}