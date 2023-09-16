/*
  ==============================================================================

    ModulationsListBoxModel.h
    Created: 11 Jun 2021 10:17:06pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "Modulation.h"
#include "ConnectionComponent.h"

using Modulation = Model::Modulation;
using namespace std;

class ModulationsListBoxModel: public ListBoxModel {
public:
  int getNumRows() override;
  ~ModulationsListBoxModel() override = default;
  Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
  void setConnections(Array<shared_ptr<Modulation>> modulationConnections);
  Slider::Listener* sliderListener;
  ConnectionComponent::Listener* delegate;
private:
  Array<shared_ptr<Modulation>> connections;

  void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override;
  void listBoxItemClicked(int row, const MouseEvent& event) override;
  void listBoxItemDoubleClicked(int row, const MouseEvent& event) override;
  void backgroundClicked(const MouseEvent& event) override;
  void selectedRowsChanged(int lastRowSelected) override;
  void deleteKeyPressed(int lastRowSelected) override;
  void returnKeyPressed(int lastRowSelected) override;
  void listWasScrolled() override;
  var getDragSourceDescription(const SparseSet<int>& rowsToDescribe) override;
  String getTooltipForRow(int row) override;
  MouseCursor getMouseCursorForRow(int row) override;
};
