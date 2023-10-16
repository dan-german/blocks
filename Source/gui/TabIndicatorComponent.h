#pragma once
#include "gui/base/BaseTabComponent.h"
#include "gui/EasingAnimator.h"

using namespace juce;

class TabIndicatorComponent: public BaseTabComponent {
private:
  EasingAnimator animator;
public:
  TabIndicatorComponent(int column, int width, GridComponent* grid);

  void animate();
  void themeChanged(Theme theme) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabIndicatorComponent)
};