/*
  ==============================================================================

    Tab.h
    Created: 18 Jan 2022 5:45:36pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "settings/Constants.h"
#include "model/Module.h"
#include "model/ModuleParameter.h"
#include "util/UniqueRandom.h"

namespace Model {
class Tab: public Module {
public:
  enum class Parameters { selection, columns };
  enum class Selection { forward, back, random, unique };

  int column = 0;
  int currentColumn = 0;
  bool state;
  UniqueRandom uniqueRandom;
  juce::Array<int> activeColumns;

  Tab(Type type, int number);

  bool getState() { return state; }
  void setState(bool _state) { this->state = _state; }
  int getLength() { return length; }
  void setLength(int _length) { this->length = _length; }
  void setColumn(int column) { this->column = column; }

  juce::Array<int> getNextColumns(int columnCount);
  juce::Array<int> getAllColumns();

private:
  int stepColumn(Selection selection);
};
}