/*
  ==============================================================================

    ModulationsListBoxModel.cpp
    Created: 11 Jun 2021 10:17:06pm
    Author:  Dan German

  ==============================================================================
*/

#include "model/connection_list_model.h"
#include "ui_utils.h"

int ModulationsListBoxModel::getNumRows() { return connections_.size(); }

Component* ModulationsListBoxModel::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) {
  ConnectionComponent* component;

  if (auto castComponent = static_cast<ConnectionComponent*>(existingComponentToUpdate))
    component = castComponent;
  else
    component = new ConnectionComponent();

  component->reset();
  if (rowNumber >= connections_.size()) return component;
  auto connection = connections_[rowNumber];

  bool envelopeToOscGain = connection->source->id.type == "envelope" && connection->target->id.type == "osc" && connection->parameter_name_ == "level";
  if (envelopeToOscGain) {
    component->handleOscGainEnvelope();
  } else {
    auto magnitude_parameter = connection->amount_parameter_;
    component->slider.juce_slider_.setRange(magnitude_parameter->min, magnitude_parameter->max);
    component->slider.juce_slider_.setValue(magnitude_parameter->value_processor->value(), dontSendNotification);
    component->slider.juce_slider_.textFromValueFunction = [magnitude_parameter](double value) { return UIUtils::getSliderTextFromValue(value, *magnitude_parameter.get()); };

    bool bipolar = static_cast<bool>(connection->bipolar_parameter_->value_processor->value());
    component->indicator.setBipolar(bipolar);
    component->bipolarButton.setState(bipolar);
  }

  auto parameter_name = connection->parameter_name_;
  auto target_name = connection->target->display_name;

  component->target.setText(target_name + " " + parameter_name, dontSendNotification);
  component->delegate = delegate_;
  component->indicator.setColour(connection->source->colour.colour);
  component->source.setText(connection->source->display_name, dontSendNotification);
  component->row = rowNumber;
  component->source.setColour(Label::ColourIds::textColourId, connection->source->colour.colour);

  return component;
}

void ModulationsListBoxModel::setConnections(std::vector<std::shared_ptr<model::Connection>> modulationConnections) {
  this->connections_.clear();
  this->connections_ = modulationConnections;
}

void ModulationsListBoxModel::paintListBoxItem(int rowNumber, Graphics& g, int width, int height, bool rowIsSelected) { }
void ModulationsListBoxModel::listBoxItemClicked(int row, const MouseEvent& event) { ListBoxModel::listBoxItemClicked(row, event); }
void ModulationsListBoxModel::listBoxItemDoubleClicked(int row, const MouseEvent& event) { ListBoxModel::listBoxItemDoubleClicked(row, event); }
void ModulationsListBoxModel::backgroundClicked(const MouseEvent& event) { ListBoxModel::backgroundClicked(event); }
void ModulationsListBoxModel::selectedRowsChanged(int lastRowSelected) { ListBoxModel::selectedRowsChanged(lastRowSelected); }
void ModulationsListBoxModel::deleteKeyPressed(int lastRowSelected) { ListBoxModel::deleteKeyPressed(lastRowSelected); }
void ModulationsListBoxModel::returnKeyPressed(int lastRowSelected) { ListBoxModel::returnKeyPressed(lastRowSelected); }
void ModulationsListBoxModel::listWasScrolled() { ListBoxModel::listWasScrolled(); }
var ModulationsListBoxModel::getDragSourceDescription(const SparseSet<int>& rowsToDescribe) { return ListBoxModel::getDragSourceDescription(rowsToDescribe); }
String ModulationsListBoxModel::getTooltipForRow(int row) { return ListBoxModel::getTooltipForRow(row); }
MouseCursor ModulationsListBoxModel::getMouseCursorForRow(int row) { return ListBoxModel::getMouseCursorForRow(row); }