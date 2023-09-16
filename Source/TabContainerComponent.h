#pragma once

#include "TabComponent.h"
#include "GridComponent.h"
#include "TabIndicatorComponent.h"
#include <juce_gui_basics/juce_gui_basics.h>

using namespace juce;

class TabContainerComponent: public GridComponent, public ComponentBoundsConstrainer {
public:
  TabContainerComponent(GridComponent::Config config);

  void paint(juce::Graphics& g) override;
  void resized() override;
  Index indexForPoint(const Point<int> point) const override;
  void hideAllItems(bool hidden, GridItemComponent* ignore = nullptr) override;
  DarkBackground darkener;
  OwnedArray<TabIndicatorComponent> glowIndicators;

  inline void animateGlowIndicators(Array<int> columns) {
    for (auto column : columns)
      glowIndicators[column]->animate();
  }
private:
  void setupGlowIndicators();
  void resizeGlowIndicators();
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabContainerComponent)
};