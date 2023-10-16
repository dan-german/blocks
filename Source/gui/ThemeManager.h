#pragma once
#include "juce_gui_basics/juce_gui_basics.h"
#include "gui/ThemeListener.h"
using namespace juce;

class ThemeManager {
private:
  Theme current;
  int index = 0;
  std::vector<ThemeListener*> listeners;

  std::vector<Theme> themes {
    Theme({ Colour(40, 40, 40), Colour(64, 64, 64), Colour(110, 110, 110), Colour(218, 218, 218), true }), // dark
    Theme({ Colour(28, 33, 40), Colour(47, 54, 63), Colour(109, 117, 125), Colour(218, 218, 218), true }), // navy
    Theme({ Colour(181, 181, 181), Colour(224, 224, 224), Colour(49, 49, 49), Colour(255, 255, 255), false }) // oreo
  };
public:
  ThemeManager() {
    current = themes[0];
  };

  int next() {
    index = (index + 1) % themes.size();
    current = themes[index];

    for (auto listener : listeners)
      if (listener != nullptr)
        listener->themeChanged(current);

    return index;
  };

  void set(int index) {
    this->index = index;
    current = themes[index];

    for (auto listener : listeners)
      listener->themeChanged(current);
  };

  Theme getCurrent() { return current; };

  inline static ThemeManager* instance;

  static ThemeManager* shared() {
    if (instance == nullptr) instance = new ThemeManager();
    return instance;
  }

  void addListener(ThemeListener* listener) {
    listeners.push_back(listener);
  }

  void removeListener(ThemeListener* listener) {
    listeners.erase(std::remove(listeners.begin(), listeners.end(), listener), listeners.end());
  }
};