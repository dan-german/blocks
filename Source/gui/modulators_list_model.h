/*
  ==============================================================================

    ModulatorsListModel.h
    Created: 30 Oct 2021 12:29:38am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
// #include "model/Module.h"
#include "gui/ModulatorComponent.h"
#include "module_new.h"

// using Module = Model::Module;

class ModulatorsListModel: public ListBoxModel, BlocksSlider::Listener {
private:
  std::vector<std::shared_ptr<model::Module>> modulators_;
  std::unordered_map<std::string, std::shared_ptr<model::Module>> model_map_;
  std::unordered_map<std::string, ModulatorComponent*> modulator_component_map_;

  void setModelToComponent(std::shared_ptr<model::Module> model, ModulatorComponent& component) const;
  void onEnvelopeAdjusted(std::shared_ptr<model::Module> module, std::string parameter_name, float value) const;
  void onLFOAdjusted(std::shared_ptr<model::Module> module, std::string parameter_name, float value) const;
  void setSliderAsFrequency(std::shared_ptr<model::Module> module, LabeledSlider* slider) const;
  void setSliderAsTempo(std::shared_ptr<model::Module> module, LabeledSlider* slider) const;
  void sliderAdjusted(BlocksSlider* slider, float value) override;
public:
  ~ModulatorsListModel() override = default;
  Component* refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) override;
  Slider::Listener* sliderListener;
  ModulatorComponent::Listener* modulator_listener;

  int getNumRows() override;
  void remove(int index);
  void setModulators(std::vector<std::shared_ptr<model::Module>> modulators);
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
