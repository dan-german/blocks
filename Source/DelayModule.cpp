#include "DelayModule.h"
#include "NoteHelper.h"
#include "ModuleParameter.h"

using namespace Model;

DelayModule::DelayModule(int number): Block(Types::delay, number) {
  category = Module::Category::effect;

  createFloatParameter({ .name = "feedback", .defaultValue = 0.3f });
  auto sync = createChoiceParameter({ "sync", { "ms", "tempo", "dotted", "triplets" }, 0 });

  auto timeRange = NormalisableRange(1.0f, 5000.0f, 0.01f);
  std::function<String(double)> textFromValueFunction = [timeRange, sync](double value) {
    if (sync->audioParameter->getValue() == 0) return String(value) + "ms";
    auto mappedValue = jmap(float(value), timeRange.start, timeRange.end, 9.0f, 0.0f);
    return NoteHelper::durationStrings[int(mappedValue)];
  };

  createFloatParameter({ .name = "time", .defaultValue = 0.0f, .textFromValueFunction = textFromValueFunction, .range = timeRange, .skew = 0.75f });
  createFloatParameter({ .name = "mix", .defaultValue = 0.4f });
}