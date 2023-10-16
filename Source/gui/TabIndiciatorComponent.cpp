#include "TabIndicatorComponent.h"

TabIndicatorComponent::TabIndicatorComponent(int column, int width, GridComponent* grid): BaseTabComponent(column, width, grid) {
  themeChanged(ThemeManager::shared()->getCurrent());
}

void TabIndicatorComponent::animate() {
  animator.reset();
  this->setVisible(true);

  auto animation = [this](float value, float) {
    float mappedValue = juce::jmap(value, 1.0f, 0.0f, 0.0f, 1.0f);
    this->setAlpha(mappedValue);
    repaint();
  };

  auto completion = [this]() {
    this->setVisible(false);
    this->setAlpha(1.0f);
    repaint();
  };

  EasingAnimator::AnimateInput input = { .seconds = 0.20f, .completion = completion, .animation = animation };
  animator.animate(input);
}

void TabIndicatorComponent::themeChanged(Theme theme) {
  bgColour = theme.dark ? theme.three : theme.one;
  repaint();
}