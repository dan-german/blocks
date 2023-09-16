#include "TabContainerComponent.h"
#include "ThemeManager.h"

TabContainerComponent::TabContainerComponent(GridComponent::Config config): GridComponent(config) {
  darkener.setInterceptsMouseClicks(false, false);
  setOpaque(false);
  setupGlowIndicators();
}

void TabContainerComponent::paint(juce::Graphics& g) {
  g.setColour(ThemeManager::shared()->getCurrent().one);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), 7.5f);
}

Index TabContainerComponent::indexForPoint(const Point<int> point) const {
  const auto moduleRect = getModuleBounds();
  float column = float(point.getX()) / (moduleRect.getWidth() + config.spacing);
  return { static_cast<int>(0), static_cast<int>(std::floor(column)) };
}

void TabContainerComponent::resized() {
  for (auto item : items)
    item->setBounds(item->getBounds());
}

void TabContainerComponent::hideAllItems(bool hidden, GridItemComponent* ignore) {
  GridComponent::hideAllItems(hidden, ignore);
}

void TabContainerComponent::setupGlowIndicators() {
  for (int column = 0; column < config.columns; column++) {
    auto tab = new TabIndicatorComponent(column, config.itemWidth, this);
    tab->setInterceptsMouseClicks(false, false);
    glowIndicators.add(tab);
  }
}

void TabContainerComponent::resizeGlowIndicators() {
  for (int column = 0; column < config.columns; column++) {
    glowIndicators[column]->setBounds(
      column * config.itemWidth + config.spacing,
      0,
      config.itemWidth,
      config.itemHeight
    );
  }
}