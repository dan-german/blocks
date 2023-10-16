#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/GridItemComponent.h"
#include "gui/ThemeManager.h"

using namespace juce;

class TabContainerComponent;
class BaseTabComponent: public GridItemComponent, ComponentBoundsConstrainer, public ThemeListener {
public:
  BaseTabComponent(int column, int width, GridComponent* grid);
  ~BaseTabComponent() override;

  void paint(juce::Graphics& g) override;
  ComponentBoundsConstrainer* getBoundsConstrainer() override { return this; }
  Rectangle<int> clipRegion;
  Component* grid;
  Colour bgColour = Colour(84, 84, 84);
  void checkBounds(Rectangle<int>& bounds, const Rectangle<int>&, const Rectangle<int>&, bool, bool, bool, bool) override;
  void themeChanged(Theme theme) override;
private:
  Path path;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BaseTabComponent)
};
