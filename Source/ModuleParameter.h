/*
  ==============================================================================

    ModuleParameter.h
    Created: 22 Apr 2021 10:14:54am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "Modulation.h"

namespace Model {
  class ModuleParameter {
  public:
    struct Input {
      RangedAudioParameter* audioParameter;
      String id;
      bool isModulatable = false;
      float skew = 1.0f;
      String valueSuffix = "";
      std::function<String(double)> textFromValueFunction = {};
      std::function<double(const String&)> valueFromTextFunction = {};
    };

    RangedAudioParameter* audioParameter;
    String id;
    bool isModulatable;
    float skew = 1.0f;
    String valueSuffix = "";
    Array<std::shared_ptr<Modulation>> connections;
    std::function<String(double)> textFromValueFunction;
    std::function<double(const String&)> valueFromTextFunction;
    ModuleParameter(RangedAudioParameter* audioParameter,
      String id,
      bool isModulatable,
      float skew = 1.0f,
      String valueSuffix = "",
      std::function<String(double)> textFromValueFunction = {},
      std::function<double(const String&)> valueFromTextFunction = {});

    ModuleParameter(Input input): ModuleParameter(input.audioParameter, input.id, input.isModulatable, input.skew, input.valueSuffix, input.textFromValueFunction, input.valueFromTextFunction) {}

    ~ModuleParameter();

    inline float get0To1Value() {
      float value = audioParameter->getValue();
      return audioParameter->convertTo0to1(value);
    }

    inline float getNormalizedValue() {
      auto range = audioParameter->getNormalisableRange();
      float value = audioParameter->getValue();
      float adjustedValue = range.convertFrom0to1(value);
      return adjustedValue;
    }

    int getIndexOfConnection(std::shared_ptr<Modulation> connection) {
      for (int i = 0; i < connections.size(); i++)
        if (connection == connections[i])
          return i;

      return -1;
    }

    void setValue(float value) {
      audioParameter->setValueNotifyingHost(audioParameter->getNormalisableRange().convertTo0to1(value));
    }
    void removeConnection(std::shared_ptr<Modulation> connection) { connections.remove(connections.indexOf(connection)); }
  };
}