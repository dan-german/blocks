/*
  ==============================================================================

    ModulatorsListModel.h
    Created: 30 Oct 2021 12:29:38am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "model/Module.h"
#include "gui/ModulatorComponent.h"

using Module = Model::Module;

class ModulatorsListModel: public ListBoxModel {
private:
  Array<std::shared_ptr<Module>> modulators;
  void setupModulatorComponent(Module& model, ModulatorComponent& component) const;
public:
  ~ModulatorsListModel() override = default;
  Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
  Slider::Listener* sliderListener;
  ModulatorComponent::Listener* modulatorListener;

  int getNumRows() override;
  void add(std::shared_ptr<Module> modulator);
  void remove(int index);
  void setModulators(Array<std::shared_ptr<Module>> modulators);
protected:
  void paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) override {}
  void listBoxItemDoubleClicked(int row, const MouseEvent& event) override;
  var getDragSourceDescription(const SparseSet<int>& rowsToDescribe) override;
  void backgroundClicked(const MouseEvent& event) override { ListBoxModel::backgroundClicked(event); }
  void selectedRowsChanged(int lastRowSelected) override { ListBoxModel::selectedRowsChanged(lastRowSelected); }
  void deleteKeyPressed(int lastRowSelected) override { ListBoxModel::deleteKeyPressed(lastRowSelected); }
  void returnKeyPressed(int lastRowSelected) override { ListBoxModel::returnKeyPressed(lastRowSelected); }
  void listWasScrolled() override { ListBoxModel::listWasScrolled(); }
  String getTooltipForRow(int row) override { return ListBoxModel::getTooltipForRow(row); }
  MouseCursor getMouseCursorForRow(int row) override { return ListBoxModel::getMouseCursorForRow(row); }
};
