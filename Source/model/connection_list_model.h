/*
  ==============================================================================

    ModulationsListBoxModel.h
    Created: 11 Jun 2021 10:17:06pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/Modulation.h"
#include "gui/ConnectionComponent.h"
#include "connection.h"

using Modulation = Model::Modulation;

class ModulationsListBoxModel: public ListBoxModel {
public:
  int getNumRows() override;
  ~ModulationsListBoxModel() override = default;
  Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
  void setConnections(std::vector<std::shared_ptr<model::Connection>> modulationConnections);
  Slider::Listener* slider_listener_;
  ConnectionComponent::Listener* delegate_;
private:
  std::vector<std::shared_ptr<model::Connection>> connections_;

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
