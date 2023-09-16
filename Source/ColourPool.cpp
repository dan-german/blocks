/*
  ==============================================================================

    ColourPool.cpp
    Created: 8 May 2021 2:13:27am
    Author:  Dan German

  ==============================================================================
*/

#include "ColourPool.h"

ColourPool::ColourPool() {
  for (int id = 0; id < colours.size(); id++)
    ids.add(id);

  for (int index = 0; index < colours.size(); index++) {
    auto randomId = Random().nextInt(Range(index, colours.size()));
    ids.swap(index, randomId);
  }
}

ModuleColour ColourPool::getRandom() {
  if (ids.isEmpty()) {
    return { }; // todo: default colour
  } else {
    auto id = ids.removeAndReturn(Random().nextInt(ids.size()));
    return { colours[id], id };
  }
}
