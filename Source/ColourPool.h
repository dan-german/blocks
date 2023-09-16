/*
  ==============================================================================

    ColourPool.h
    Created: 8 May 2021 2:13:27am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "ModuleColour.h"
#include <juce_gui_basics/juce_gui_basics.h>
using namespace juce;

class ColourPool {
private:
  Colour a = Colour(22, 57, 255);
  Colour b = Colour(22, 255, 199);
  Colour c = Colour(26, 238, 22);
  Colour d = Colour(88, 56, 248);
  Colour e = Colour(121, 141, 255);
  Colour f = Colour(122, 255, 128);
  Colour g = Colour(164, 200, 255);
  Colour h = Colour(181, 255, 23);
  Colour i = Colour(185, 106, 255);
  Colour j = Colour(186, 106, 255);
  Colour k = Colour(217, 30, 30);
  Colour m = Colour(227, 125, 211);
  Colour n = Colour(240, 231, 150);
  Colour l = Colour(250, 92, 92);
  Colour o = Colour(255, 237, 41);

  const Array<Colour> colours { a, b, c, d, e, f, g, h, i, j, k, m, n, l, o };

  Array<int> ids;

  inline Colour getColourForId(int id) { return (id == -1) ? getRandom().colour : colours[id]; }
public:
  ColourPool();
  ModuleColour getRandom();

  inline void retire(int id) {
    ids.add(id);
  }

  ModuleColour get(int id) {
    return id == -1 ? getRandom() : ModuleColour { getColourForId(id), id };
  }
};
