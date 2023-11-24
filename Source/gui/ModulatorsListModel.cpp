/*
  ==============================================================================

    ModulatorsListModel.cpp
    Created: 30 Oct 2021 12:29:38am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ModulatorsListModel.h"
#include "model/ModuleParameter.h"
#include "model/LFOModule.h"
#include "module_new.h"

int ModulatorsListModel::getNumRows() { return modulators.size(); }
void ModulatorsListModel::listBoxItemDoubleClicked(int row, const MouseEvent& event) { ListBoxModel::listBoxItemDoubleClicked(row, event); }
var ModulatorsListModel::getDragSourceDescription(const SparseSet<int>& rowsToDescribe) { return ListBoxModel::getDragSourceDescription(rowsToDescribe); }
void ModulatorsListModel::remove(int index) { 
  modulators.erase(modulators.begin() + index);
}

Component* ModulatorsListModel::refreshComponentForRow(int rowNumber, bool isRowSelected, Component* existingComponentToUpdate) {
  ModulatorComponent* component;

  if (auto castComponent = static_cast<ModulatorComponent*>(existingComponentToUpdate))
    component = castComponent;
  else
    component = new ModulatorComponent();

  if (modulators.size() == 0) return component;
  if (rowNumber >= modulators.size()) return component;

  setupModulatorComponent(*modulators[rowNumber], *component);
  component->row = rowNumber;

  return component;
}

void ModulatorsListModel::setupModulatorComponent(model::Module& model, ModulatorComponent& component) const {
  component.title.setText(model.name, dontSendNotification);

  for (int i = 0; i < model.parameters_.size(); i++) {
    auto parameter = model.parameters_[i];
    auto slider = component.sliders[i];
    // auto audioParameter = parameter->audioParameter;

    // slider->boxSlider.slider.setRange(audioParameter->getNormalisableRange().start,
    //   audioParameter->getNormalisableRange().end,
    //   audioParameter->getNormalisableRange().interval);

    // auto value = audioParameter->getNormalisableRange().convertFrom0to1(audioParameter->getValue());
    // slider->boxSlider.slider.setTextValueSuffix(parameter->valueSuffix);
    // slider->boxSlider.choices = parameter->audioParameter->getAllValueStrings();
    // slider->boxSlider.slider.setSkewFactor(parameter->skew, false);

    // slider->label.setText(parameter->id, dontSendNotification);

    // if (dynamic_cast<AudioParameterFloat*>(audioParameter))
    //   slider->boxSlider.slider.setNumDecimalPlacesToDisplay(2);
    // else
    //   slider->boxSlider.slider.setNumDecimalPlacesToDisplay(0);

    // auto choices = audioParameter->getAllValueStrings();

    // if (parameter->textFromValueFunction) {
    //   slider->boxSlider.slider.textFromValueFunction = parameter->textFromValueFunction;
    //   slider->boxSlider.slider.setNumDecimalPlacesToDisplay(0);
    // } else if (choices.size() != 0)
    //   slider->boxSlider.slider.textFromValueFunction = [choices](double value) { return choices[value]; };
    // else {
    //   slider->boxSlider.slider.textFromValueFunction = {};
    // }

    // slider->boxSlider.slider.setValue(value, dontSendNotification);
    // slider->boxSlider.slider.getTextFromValue(value);
    // slider->boxSlider.valueLabel.setText(slider->boxSlider.slider.getTextFromValue(value), dontSendNotification);
  }

  component.delegate = modulatorListener;
  component.setColour(model.colour.colour);

  if (model.id.type == Model::Types::lfo) {
    component.oscillatorPainter.setVisible(true);
    // auto parameter = model.parameter(Model::LFOModule::Parameters::pWaveform);
    // auto value = parameter->audioParameter->getNormalisableRange().convertFrom0to1(parameter->audioParameter->getValue());
    // component.oscillatorPainter.setWaveformType(static_cast<OscillatorPainter::WaveformType>((int)value));
    // component.envelopePath.setVisible(false);
  } else {
    component.oscillatorPainter.setVisible(false);
    component.envelopePath.setVisible(true);
  }

  // todo: update the skew only after rate value changes... atm it looks weird
  if (model.id.type == Model::Types::lfo) {
    component.onSliderValueChange = [&component](int index, float value) {
      if (index == 0) {
        component.oscillatorPainter.setWaveformType(static_cast<OscillatorPainter::WaveformType>(int(value)));
      } else if (index == 1) {
        if (int(value) == 0) {
          component.sliders[2]->boxSlider.slider.setSkewFactor(0.25);
        } else {
          component.sliders[2]->boxSlider.slider.setSkewFactor(1);
        }
      }
    };
  } else if (model.id.type == Model::Types::adsr) {
    component.onSliderValueChange = [&component, &model](int index, float value) {
      // auto normalizedValue = model.parameters[index]->audioParameter->convertTo0to1(value);
      // switch (index) {
      // case 0: component.envelopePath.setAttack(normalizedValue); break;
      // case 1: component.envelopePath.setDecay(normalizedValue); break;
      // case 2: component.envelopePath.setSustain(normalizedValue); break;
      // case 3: component.envelopePath.setRelease(normalizedValue); break;
      // default: break;
      // }
    };
  }
}

void ModulatorsListModel::setModulators(std::vector<std::shared_ptr<model::Module>> modulators) {
  this->modulators.clear();
  this->modulators = modulators;
}