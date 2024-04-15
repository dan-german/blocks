/*
  ==============================================================================

    Tab.cpp
    Created: 18 Jan 2022 5:45:36pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/Tab.h"
#include "model/ModuleParameter.h"

using Tab = Model::Tab;

Tab::Tab(Type type, int number): Module(type, number), uniqueRandom(0, Constants::columns, 1) {
  category = Module::Category::tab;

  StringArray selectionOptions = { "forward", "back", "random" };
  createChoiceParameter({ "selection", selectionOptions, 0, false });
  createIntParameter({ .name = "columns", .min = 1, .max = Constants::columns, .defaultValue = 1, .isModulatable = false });

  srand(time(NULL));
}

Array<int> Tab::getNextColumns(int columnCount) {
  activeColumns.clear();

  auto selection = static_cast<Selection>(parameter(static_cast<int>(Parameters::selection))->getNormalizedValue());

  for (int i = 0; i < columnCount; i++) {
    activeColumns.add(column + stepColumn(selection));
  }

  return activeColumns;
}

int Tab::stepColumn(Selection selection) {
  switch (selection) {
  case Selection::forward: {
    if (++currentColumn >= length) currentColumn = 0;
    return currentColumn;
  }
  case Selection::back: {
    if (--currentColumn < 0) currentColumn = length - 1;
    return currentColumn;
  }
  case Selection::random:
    return rand() % length;
  case Selection::unique: // todo - implement
    return rand() % length;
  default: jassertfalse;
  }
  return 0;
}

Array<int> Tab::getAllColumns() {
  Array<int> columns;

  for (int i = 0; i < length; i++)
    columns.add(column + i);

  return columns;
}