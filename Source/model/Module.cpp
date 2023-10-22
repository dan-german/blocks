/*
  ==============================================================================

    Module.cpp
    Created: 17 Nov 2020 12:39:03am
    Author:  Dan German

  ==============================================================================
*/

#include "model/Module.h"
#include "model/ModuleParameter.h"

namespace Model {
  Module::Module(Type type, int number) {
    this->id = { type, number };
    this->name = type + " " + String(number).toStdString();
  }

  void Module::reset() {
    for (auto parameter : parameters) {
      auto defaultValue = parameter->audioParameter->getDefaultValue();
      parameter->audioParameter->setValue(defaultValue);
      parameter->connections.clear();
    }

    isActive = false;
    length = 1;
    colour = { Colour(204, 201, 184), -1 };
  }

  void Module::removeConnection(std::shared_ptr<Modulation> connection) { parameter(connection->parameterIndex)->removeConnection(connection); }
  bool Module::operator<(const Module& r) const { return id.number < r.id.number; }

  void Module::createIntParameter(Module::CreateParameterIntInput input) {
    auto audioParameter = new AudioParameterInt({ name + " " + input.name, 1 }, input.name, input.min, input.max, input.defaultValue);
    createParameter(audioParameter, input.name, input.isModulatable);
  }

  void Module::createFloatParameter(Module::CreateParameterFloatInput input) {
    auto audioParameter = new AudioParameterFloat({ name + " " + input.name, 1 }, input.name, input.range, input.defaultValue);
    createParameter(audioParameter, input.name, input.isModulatable, input.skew, input.valueSuffix, input.textFromValueFunction);
  }

  ModuleParameter* Module::createChoiceParameter(Module::CreateParameterChoiceInput input) {
    auto audioParameter = new AudioParameterChoice({ name + " " + input.name, 1 }, input.name, input.choices, input.defaultIndex);
    return createParameter(audioParameter, input.name, input.isModulatable);
  }

  ModuleParameter* Module::createParameter(RangedAudioParameter* audioParameter, String name, bool isAutomatable, float skew, String valueSuffix, std::function<String(float)> textFromValueFunction) {
    auto shared = std::make_shared<ModuleParameter>(audioParameter, name, isAutomatable, skew, valueSuffix, textFromValueFunction);
    parameters.add(shared);
    parameterMap[name] = shared;
    return shared.get();
  }
}
