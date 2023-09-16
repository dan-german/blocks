#pragma once
#include "BaseTabComponent.h"
#include "Tab.h"
#include "ThemeManager.h"

using namespace juce;
using Tab = Model::Tab;

class TabComponent: public BaseTabComponent {
public:
  TabComponent(int column, int width, GridComponent* grid);
  ~TabComponent() override;
  Label label;

  void resized() override;
  void paint(juce::Graphics& g) override;
  static TabComponent* create(Tab& tab, TabContainerComponent* container);

  bool stretchable = true;

  void drawStretchIndicator(juce::Graphics& g);
  void setHidden(bool hidden) override;
  void setSelected(bool selected) override;
  void themeChanged(Theme theme) override;
private:
  Path path;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TabComponent)
};