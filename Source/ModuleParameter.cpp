/*
  ==============================================================================

    ModuleParameter.cpp
    Created: 22 Apr 2021 10:14:54am
    Author:  Dan German

  ==============================================================================
*/

#include "ModuleParameter.h"

Model::ModuleParameter::~ModuleParameter() { }

Model::ModuleParameter::ModuleParameter(RangedAudioParameter* audioParameter,
  String id,
  bool isModulatable,
  float skew,
  String valueSuffix,
  std::function<String(double)> textFromValueFunction,
  std::function<double(const String&)> valueFromTextFunction)
  : audioParameter(audioParameter),
  id(id),
  isModulatable(isModulatable),
  skew(skew),
  valueSuffix(valueSuffix),
  textFromValueFunction(textFromValueFunction),
  valueFromTextFunction(valueFromTextFunction)
{
}