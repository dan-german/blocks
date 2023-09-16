#include <map>
#include "ModelConstants.h"

using namespace Model;
using namespace std;

template <class M>
struct ModuleContainer {
  map<Type, Array<shared_ptr<M>>> map;
  Array<shared_ptr<M>> all;

  void spawn(Array<Type> types, function<shared_ptr<M>(Type, int)> spawner) {
    for (const auto& type : types) {
      Array<shared_ptr<M>> array;

      for (int i = 1; i <= MAX_MODULES_PER_TYPE; i++) {
        array.add(shared_ptr<M>(spawner(type, i)));
      }

      map[type] = array;
      all.addArray(array);
    }
  }

  void retire(shared_ptr<M> module) {
    module->reset();
    map[module->id.type].add(module);

    struct Sorter
    {
      bool operator() (const shared_ptr<Module> a, const shared_ptr<Module> b) const noexcept { return a->id.number < b->id.number; }
    };

    std::sort(map[module->id.type].begin(), map[module->id.type].end(), Sorter());
  }

  shared_ptr<M> get(ID id) {
    for (int i = 0; i < map[id.type].size(); i++)
      if (map[id.type][i]->id.number == id.number)
        return map[id.type].removeAndReturn(i);

    return map[id.type].removeAndReturn(0);
  }
};