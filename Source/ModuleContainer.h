#pragma once
#include <map>
#include "model/ModelConstants.h"
#include "model/id.h"

template <class M>
struct ModuleContainer {
  std::map<std::string, juce::Array<std::shared_ptr<M>>> map;
  juce::Array<std::shared_ptr<M>> all;

  void spawn(juce::Array<std::string> types, std::function<std::shared_ptr<M>(std::string, int)> spawner, int count = model::MAX_MODULES_PER_TYPE) {
    for (const auto& type : types) {
      juce::Array<std::shared_ptr<M>> array;

      for (int i = 1; i <= count; i++) {
        array.add(std::shared_ptr<M>(spawner(type, i)));
      }

      map[type] = array;
      all.addArray(array);
    }
  }

  void retire(std::shared_ptr<M> module) {
    module->reset();
    map[module->id.type].add(module);
    struct Sorter { bool operator() (const std::shared_ptr<M> a, const std::shared_ptr<M> b) const noexcept { return a->id.number < b->id.number; } };
    std::sort(map[module->id.type].begin(), map[module->id.type].end(), Sorter());
  }

  std::shared_ptr<M> get(ID id) {
    for (int i = 0; i < map[id.type].size(); i++)
      if (map[id.type][i]->id.number == id.number)
        return map[id.type].removeAndReturn(i);

    return map[id.type].removeAndReturn(0);
  }
};