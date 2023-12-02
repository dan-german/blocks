/*
  ==============================================================================

    ModulationsListBoxModel.cpp
    Created: 11 Jun 2021 10:17:06pm
    Author:  Dan German

  ==============================================================================
*/

#include "model/connections_list_model.h"
#include "model/ModuleParameter.h"
#include "model/Module.h"
#include "OscillatorModule.h"

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

  auto magnitude_parameter = connection->magnitude_parameter_;
  component->slider.setRange(magnitude_parameter->min, magnitude_parameter->max);

  component->slider.setValue(magnitude_parameter->val->value(), dontSendNotification);
  // component->source.setText(connection->source->name, dontSendNotification);

  // String targetTitle = connection->target->name + " " + connection->target->parameters[connection->parameterIndex]->id;

  // component->target.setText(targetTitle, dontSendNotification);
  // component->slider.setNumDecimalPlacesToDisplay(3);
  component->slider.addListener(this->slider_listener_);
  // component->delegate = delegate;
  // component->indicator.setColour(connection->source->colour.colour);

  // bool bipolar = static_cast<bool>(connection->bipolarParameter->getValue());
  // component->indicator.setBipolar(bipolar);
  // component->bipolarButton.setState(bipolar);

  // using Parameters = Model::OscillatorModule::Parameters;
  // bool envelopeToOscGain = connection->source->isEnvelope() && connection->target->isOscillator() && Parameters(connection->parameterIndex) == Parameters::pGain;
  // if (envelopeToOscGain) component->handleOscGainEnvelope();

  // component->row = rowNumber;

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