/*
  ==============================================================================

    Module.h
    Created: 17 Nov 2020 12:39:03am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "model/Index.h"
#include "model/Modulation.h"
#include "gui/ModuleColour.h"
#include <vector>
#include <juce_core/juce_core.h>
#include "model/id.h"

using namespace juce;

namespace Model {
  using Type = std::string;

  namespace Types {
    inline const Type lfo = "lfo";
    inline const Type adsr = "adsr";

    inline const Type osc = "osc";
    inline const Type filter = "filter";
    inline const Type reverb = "reverb";
    inline const Type delay = "delay";
    inline const Type drive = "drive";
    inline const Type mixer = "mixer";

    inline const Type noteTab = "note";

    inline const Array<Type> all = { osc, lfo, adsr, filter, reverb, delay, drive, mixer };

    inline const Array<Type> modulators = { lfo, adsr };
    inline const Array<Type> blocks = { osc, filter, reverb, delay, drive, mixer };
    inline const Array<Type> tabs = { noteTab };
  }

  class ModuleParameter;
  class Module {
  public:
    enum Category { source, effect, modulator, tab };
    Category category;

    ID id;
    String name;
    std::map<String, std::shared_ptr<ModuleParameter>> parameterMap; // relevant only for automation? 
    Array<std::shared_ptr<ModuleParameter>> parameters;
    ModuleColour colour = { Colour(237, 237, 237), -1 };
    bool isActive = false;
    bool isChild = false;
    int length = 1;

    Module(Type type, int number);

    inline bool isModulator() { return category == Category::modulator; }
    inline bool isEnvelope() { return id.type == Types::adsr; }
    inline bool isOscillator() { return id.type == Types::osc; }

    virtual void reset();
    void removeConnection(std::shared_ptr<Modulation> connection);
    bool operator<(const Module& r) const;
    inline std::shared_ptr<ModuleParameter> parameter(int index) { return parameters[(unsigned long)index]; }

  protected:
    struct CreateParameterFloatInput;
    void createFloatParameter(Module::CreateParameterFloatInput input);

    struct CreateParameterIntInput;
    void createIntParameter(Module::CreateParameterIntInput input);

    struct CreateParameterChoiceInput;
    ModuleParameter* createChoiceParameter(Module::CreateParameterChoiceInput input);
  private:
    ModuleParameter* createParameter(RangedAudioParameter* audioParameter, String name, bool isAutomatable, float skew = 1.0f, String valueSuffix = "", std::function<String(float)> textFromValueFunction = {});
  };
}

namespace Model {
  struct Module::CreateParameterIntInput {
    String name;
    int min;
    int max;
    int defaultValue;
    std::function<String(int)> textFromValueFunction = {};
    String valueSuffix = "";
    bool isModulatable = true;
  };

  struct Module::CreateParameterFloatInput {
    String name;
    float defaultValue;
    std::function<String(float)> textFromValueFunction = {};
    NormalisableRange<float> range = { 0.0f, 1.0f, 0.0001f };
    String valueSuffix = "";
    float skew = 1.0f;
    bool isModulatable = true;
  };

  struct Module::CreateParameterChoiceInput {
    String name;
    StringArray choices;
    int defaultIndex;
    bool isModulatable = true;
  };
}
