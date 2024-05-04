#include "gui/base/BaseTabComponent.h"

#include "gui/TabContainerComponent.h"

BaseTabComponent::BaseTabComponent(int column, int width, GridComponent* grid): GridItemComponent({ Index(0, column), width }) {
  setOpaque(false);
  this->grid = grid;
  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
}

BaseTabComponent::~BaseTabComponent() {
  ThemeManager::shared()->removeListener(this);
}

void BaseTabComponent::paint(juce::Graphics& g) {
  const juce::Graphics::ScopedSaveState save(g);
  auto parentRect = getLocalArea(grid, grid->getLocalBounds());
  path.clear();
  path.addRoundedRectangle(parentRect.toFloat(), 7.5f, 7.5f);
  g.reduceClipRegion(path);
  g.fillAll(bgColour);
}

void BaseTabComponent::checkBounds(Rectangle<int>& bounds, const Rectangle<int>&, const Rectangle<int>&, bool, bool, bool, bool) {
  auto itemWidth = config.width;
  auto x = std::clamp(bounds.getX(), grid->getX(), grid->getX() + grid->getWidth() - itemWidth * length);
  bounds.setPosition(x, grid->getY());
}

void BaseTabComponent::themeChanged(Theme theme) {
  bgColour = theme.two;
  repaint();
}