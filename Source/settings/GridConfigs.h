/*
 ==============================================================================

 ModuleComponentConstants.h
 Created: 15 Aug 2020 7:52:16pm
 Author:  dange

 ==============================================================================
 */

#pragma once
#include "gui/GridComponent.h"
#include "settings/Constants.h"
using namespace juce;

namespace GridConfigs {
  inline const GridComponent::Config tab = { Constants::tabHeight, Constants::Functions::tabWidth(), 0, 0, 1, Constants::columns, false };
  inline const GridComponent::Config blocks = { Constants::moduleHeight, Constants::moduleWidth, Constants::gridEdgeSpacing, Constants::moduleSpacing, Constants::rows, Constants::columns, true };
}