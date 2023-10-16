#include <math.h>
#include "gui/GridComponent.h"
#include "gui/ThemeManager.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

Rectangle<int> GridComponent::getModuleBounds() const { return Rectangle<int>(config.itemWidth, config.itemHeight); }
GridItemComponent* GridComponent::getModuleComponent(Index index) { return itemMatrix[index.row][index.column]; }
void GridComponent::paint(Graphics&) {}
void GridComponent::resnapBlock(GridItemComponent* moduleComponent) { snapItem(moduleComponent, moduleComponent->index, true); }
void GridComponent::show() { Desktop::getInstance().getAnimator().animateComponent(this, getBounds(), 1, 140, false, 1, 1); }
void GridComponent::hide() { Desktop::getInstance().getAnimator().animateComponent(this, getBounds(), 0, 250, false, 1, 1); }
void GridComponent::setConfig(Config config) { this->config = config; }

GridComponent::GridComponent(Config config) {
  this->config = config;

  for (int row = 0; row < config.rows; row++) {
    itemMatrix.push_back(std::vector<GridItemComponent*>());
    for (int column = 0; column < config.columns; column++)
      itemMatrix[row].push_back(nullptr);
  }

  addAndMakeVisible(addButton);

  setInterceptsMouseClicks(true, true);
}

void GridComponent::snapItem(GridItemComponent* item, Index index, bool resetBounds) {
  item->index = index;

  auto bounds = boundsForItem(item, resetBounds);
  item->setBounds(bounds);
  setItemLength(item, item->length);
  itemMatrix[index.row][index.column] = item;
}

Rectangle<int> GridComponent::boundsForItem(GridItemComponent* item, bool resetBounds) {
  bool childOfAnotherComponent = item->getParentComponent() != this; // TODO: can this be done better?

  const auto localPoint = pointForIndex(Index(item->index.row, item->index.column));
  const auto globalPoint = Point<int>(localPoint.getX() + getX(), localPoint.getY() + getY());
  const auto finalPoint = childOfAnotherComponent ? globalPoint : localPoint;

  const int itemWidth = resetBounds ? getModuleBounds().getWidth() : item->getWidth();
  return Rectangle<int>(finalPoint.getX(), finalPoint.getY(), itemWidth, config.itemHeight);
}

void GridComponent::setItemLength(GridItemComponent* moduleComponent, int length) {
  auto moduleRect = getModuleBounds();
  auto width = moduleRect.getWidth() * length + config.spacing * (length - 1);
  moduleComponent->setBounds(moduleComponent->getX(), moduleComponent->getY(), width, moduleRect.getHeight());

  currentPlaceholderLength = 1;
  previousPlaceholderIndex = {};
}

Index GridComponent::indexForPoint(const Point<int> point) const {
  const auto moduleRect = getModuleBounds();
  float column = float(point.getX()) / (moduleRect.getWidth() + config.spacing * 1.5f);
  float row = float(point.getY()) / (moduleRect.getHeight() + config.spacing * 1.5f);
  return { static_cast<int>(std::floor(row)), static_cast<int>(std::floor(column)) };
}

Point<int> GridComponent::pointForIndex(Index index) const {
  int row = index.row;
  int column = index.column;
  const int x = config.edgeSpacing + config.itemWidth * column + column * config.spacing;
  const int y = config.edgeSpacing + config.itemHeight * row + row * config.spacing;
  return Point<int>(x, y);
}

void GridComponent::mouseMove(const MouseEvent& event) {
  auto indexUnderMouse = indexForPoint(event.getPosition());
  if (indexUnderMouse == previousIndexUnderMouse) { return; }
  if (indexUnderMouse.row >= config.rows || indexUnderMouse.column >= config.columns) { return; }
  previousIndexUnderMouse = indexUnderMouse;

  showAddButton(indexUnderMouse);
}

void GridComponent::showAddButton(Index index) {
  if (itemMatrix[index.row][index.column] != nullptr) { return; }
  auto point = pointForIndex(index);
  auto moduleBounds = getModuleBounds();
  int size = 10;
  int x = point.getX() + moduleBounds.getWidth() / 2 - size / 2;
  int y = point.getY() + moduleBounds.getHeight() / 2 - size / 2;
  addButton.setBounds(x, y, size, size);
  addButton.setAlpha(1);
}

void GridComponent::mouseExit(const MouseEvent& event) {
  addButton.setAlpha(0);
  previousIndexUnderMouse = { -1, -1 };
}

void GridComponent::detachModule(Index index, bool shouldRemove) {
  auto item = itemMatrix[index.row][index.column];
  itemMatrix[index.row][index.column] = {};

  if (shouldRemove) items.remove(items.indexOf(item));
}

void GridComponent::clear() {
  for (int i = items.size() - 1; i >= 0; i--) {
    detachModule(items[i]->index);
  }

  items.clear();
}

bool GridComponent::isIndexValid(Index currentIndex, Index targetIndex, int length) {
  if (length + targetIndex.column > config.columns) return false;

  for (int i = 1; i < length; i++) {
    auto module = getModuleComponent(Index(targetIndex.row, targetIndex.column + i));
    if (module && module->index != currentIndex)
      return false;
  }

  return true;
}

GridItemComponent* GridComponent::isSlotTaken(Index index, GridItemComponent* candidate) {
  for (int i = index.column; i >= 0; i--) {
    auto candidateIndex = Index(index.row, i);
    auto module = getModuleComponent(candidateIndex);

    if (module && module != candidate) {
      bool moduleIsStretchedOverTargetIndex = module->index.column + module->length > index.column;
      if (moduleIsStretchedOverTargetIndex) {
        return module;
      }
    }
  }

  return nullptr;
}

GridItemComponent* GridComponent::isSlotTaken(Index index) {
  for (int i = index.column; i >= 0; i--) {
    auto candidateIndex = Index(index.row, i);
    auto module = getModuleComponent(candidateIndex);

    if (module) {
      bool moduleIsStretchedOverTargetIndex = module->index.column + module->length > index.column;
      if (moduleIsStretchedOverTargetIndex)
        return module;
    }
  }

  return nullptr;
}

void GridComponent::mouseUp(const MouseEvent& event) {
  if (listener) listener->clickedOnGrid(this, indexForPoint(event.getPosition()));
}

void GridComponent::addItem(GridItemComponent* item, Index index, bool resetBounds) {
  item->listener = this;
  item->grid = this;
  items.add(item);
  itemMatrix[index.row][index.column] = item;
  snapItem(item, index, resetBounds);
}

void GridComponent::gridItemEndedDrag(GridItemComponent* item, const MouseEvent& mouseEvent) {
  auto position = getItemPosition(item);
  auto index = indexForPoint(position);

  if (isIndexOutside(index)) {
    itemLandedOutside(item, index);
    listener->gridItemRemoved(this, item);
  } else {
    auto index = indexForPoint(position);
    handleItemLandedInside(item, index);
  }

  listener->gridItemEndedDrag(this, item, mouseEvent);
}

void GridComponent::handleItemLandedInside(GridItemComponent* item, Index index) {
  bool remainedInSamePosition = index == item->index;
  if (remainedInSamePosition) {
    resnapBlock(item);
    return;
  }

  if (isSlotTaken(index, item)) {
    resnapBlock(item);
    return;
  }

  auto oldIndex = item->index;

  if (!isIndexValid(oldIndex, index, item->length)) {
    resnapBlock(item);
    return;
  }

  detachModule(oldIndex, false);
  snapItem(item, index);
  listener->gridItemRepositioned(this, item, oldIndex);
}

void GridComponent::gridItemIsStretching(GridItemComponent* item, int offset) {
  if (offset <= 1) return;
  item->setBounds(item->getBounds().withWidth(offset + item->config.width));

  const float numberOfModulesStretchedOver = offset / (float)(getModuleBounds().getWidth() + config.spacing);
  const float offsetFraction = (numberOfModulesStretchedOver - floor(numberOfModulesStretchedOver));
  const float aboveThreshold = offsetFraction > Constants::moduleGridSnapThreshold;
  const int columnsStretchedOver = aboveThreshold ? (int)std::ceil(numberOfModulesStretchedOver) : (int)std::floor(numberOfModulesStretchedOver);

  int newLength = std::clamp(columnsStretchedOver + 1, 1, Constants::columns - item->index.column);
  if (newLength == item->length) return;

  auto difference = newLength - item->length;
  if (difference == 0) return;

  if (isIndexOutside(item->index)) {
    itemHovered(item, false, false, newLength, item->index);
    return;
  }

  bool isSlotInvalid = isSlotTaken(item->index, item) != nullptr || !isIndexValid(item->index, item->index, newLength);

  itemHovered(item, !isSlotInvalid, true, newLength, item->index);
  if (isSlotInvalid) return;
  item->length = newLength;
  listener->gridItemLengthChanged(this, item, newLength);
}

void GridComponent::gridItemStretchEnded(GridItemComponent* item, int offset) {
  setItemLength(item, item->length);
}

void GridComponent::gridItemClicked(GridItemComponent* item, const MouseEvent& event) {
  listener->gridItemClicked(this, item, event);
}

void GridComponent::gridItemIsDragging(GridItemComponent* item, const MouseEvent& mouseEvent) {
  auto position = getItemPosition(item);
  auto index = indexForPoint(position);

  if (previousPlaceholderIndex && *previousPlaceholderIndex == index) { return; }
  if (index.column >= config.columns || index.row >= config.rows || index.column < 0 || index.row < 0) return;

  listener->gridItemHovered(this, item, index);
  bool isSlotInvalid = isSlotTaken(index, item) != nullptr || !isIndexValid(item->index, index, item->length);

  itemHovered(item, !isSlotInvalid, true, item->length, index);
  previousPlaceholderIndex = index;
}

void GridComponent::gridItemStartedDrag(GridItemComponent* item, const MouseEvent& mouseEvent) {
  listener->gridItemStartedDrag(this, item, mouseEvent);
}

void GridComponent::hideAllItems(bool hidden, GridItemComponent* ignore) {
  for (auto item : items) {
    if (item == ignore) continue;
    item->setHidden(hidden);
  }
}

Point<int> GridComponent::getItemPosition(GridItemComponent* item) {
  auto itemPosition = getLocalPoint(getParentComponent(), item->getPosition());
  auto x = itemPosition.getX() + getModuleBounds().getWidth() / 2; // mid point of the leftmost item
  auto y = itemPosition.getY() + item->getHeight() / 2; // mid y 
  return Point(x, y);
}