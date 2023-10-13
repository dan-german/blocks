#include "BlockGridComponent.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

void BlockGridComponent::highlightColumn(int start, int end) {
  highlight = true;
  GraphicsTimer::start();
  this->firstHighlightedColumn = start;
  this->lastHighlightedColumn = end;

  setDotsHidden(true);
  hideAllItems(true);

  for (int column = start; column < end; column++) {
    for (int row = 0; row < config.rows; row++) {
      setItemHidden({ row, column }, false);
    }
  }
}

void BlockGridComponent::reset() {
  for (auto dotArray : dots)
    for (auto dot : *dotArray)
      dot->setColour(DotComponent::defaultColour);

  hideAllItems(false);
  highlight = false;
  GraphicsTimer::stop();
}

void BlockGridComponent::update(const float secondsSinceLastUpdate) {
  if (!highlight) return;

  float speed = 6;
  sineX += secondsSinceLastUpdate * speed;

  if (sineX >= M_PI * 2.0f) {
    sineX -= M_PI * 2.0f;
  }

  for (int row = 0; row <= config.rows; row++) {
    float offset = 0.3f * row;
    float y = std::sin(sineX - offset);
    float alpha = jmap(y, -1.0f, 1.0f, 0.1f, 2.0f);
    for (int column = firstHighlightedColumn; column < lastHighlightedColumn + 1; column++) {
      auto dot = dots.getUnchecked(row)->getUnchecked(column);
      dot->setColour(Colour(95, 95, 95).brighter(alpha));
    }
  }
}

BlockGridComponent::BlockGridComponent(GridComponent::Config config): GridComponent(config) {
  addAndMakeVisible(blockPlaceholder);
  spawnDots();
}

void BlockGridComponent::spawnDots() {
  int size = Constants::gridDotSize;

  for (int row = 0; row <= config.rows; row++) {
    dots.add(new OwnedArray<DotComponent>());
    positions.add(Array<Point<int>>());
    for (int column = 0; column <= config.columns; column++) {
      auto dot = new DotComponent();
      dots[row]->add(dot);
      addAndMakeVisible(dot);

      auto point = pointForIndex(Index(row, column));

      int x = point.getX() - config.edgeSpacing;
      int y = point.getY() - config.edgeSpacing;

      dot->setBounds(x, y, size, size);
      dot->setEnabled(true);
    }
  }
}

void BlockGridComponent::gridItemStretchEnded(GridItemComponent* item, int offset) {
  GridComponent::gridItemStretchEnded(item, offset);
  blockPlaceholder.setVisible(false);
  setItemLength(item, item->length);
  auto block = dynamic_cast<BlockComponent*>(item);
  block->animate();
}

void BlockGridComponent::setDotsHidden(bool hidden) {
  for (auto dotArray : dots) {
    for (auto dot : *dotArray) {
      dot->setColour(hidden ? DotComponent::defaultColour.withAlpha(0.5f) : DotComponent::defaultColour);
    }
  }
}

void BlockGridComponent::resetDots() {
  for (auto dotArray : dots) {
    for (auto dot : *dotArray) {
      dot->setVisible(true);
    }
  }

  for (auto item : items) {
    auto index = item->index;
    for (int column = index.column; column <= index.column + item->length; column++) {
      for (int row = 0; row <= 1; row++) {
        dots.getUnchecked(index.row + row)->getUnchecked(column)->setVisible(true);
      }
    }
  }

  for (auto item : items) {
    if (item->length <= 1) continue;

    for (int column = 1; column < item->length; column++) {
      for (int row = 0; row <= 1; row++) {
        auto dot = dots.getUnchecked(item->index.row + row)->getUnchecked(item->index.column + column);
        dot->setVisible(false);
      }
    }
  }
}

void BlockGridComponent::snapItem(GridItemComponent* item, Index index, bool resetBounds) {
  GridComponent::snapItem(item, index, resetBounds);
  resetDots();
  auto block = dynamic_cast<BlockComponent*>(item);
  block->animate();
}

void BlockGridComponent::resizeDots() {
  for (int row = 0; row <= config.rows; row++) {
    for (int column = 0; column <= config.columns; column++) {
      auto dot = dots.getUnchecked(row)->getUnchecked(column);
      auto point = pointForIndex(Index(row, column));

      int x = point.getX() - config.edgeSpacing;
      int y = point.getY() - config.edgeSpacing;

      dot->setBounds(x, y, dot->getWidth(), dot->getHeight());
    }
  }
}

void BlockGridComponent::resized() {
  resizeDots();
}

void BlockGridComponent::hideDotsAroundIndex(GridItemComponent* blockComponent, Index index, int length, bool visible) {
  if (previousPlaceholderIndex.has_value() && !isIndexInside(*previousPlaceholderIndex)) return;
  for (int column = index.column; column < index.column + 1 + length; column++)
    for (int row = 0; row <= 1; row++)
      dots.getUnchecked(index.row + row)->getUnchecked(column)->setVisible(visible);

  for (auto block : items) {
    if (block->length <= 1 || block == blockComponent) continue;

    for (int column = 1; column < block->length; column++) {
      for (int row = 0; row <= 1; row++) {
        auto dot = dots.getUnchecked(block->index.row + row)->getUnchecked(block->index.column + column);
        dot->setVisible(false);
      }
    }
  }
}

void BlockGridComponent::itemHovered(GridItemComponent* item, bool valid, bool inside, int proposedLength, Index index) {
  resetDots();
  auto length = proposedLength;

  if (!inside) {
    hideDotsAroundIndex(item, *previousPlaceholderIndex, length, true);
    blockPlaceholder.setVisible(false);
    previousPlaceholderIndex = index;
    return;
  }

  blockPlaceholder.setVisible(true);
  blockPlaceholder.setColour(!valid);

  length = std::clamp(length, 1, config.columns - index.column);
  blockPlaceholder.setLength(length);

  hideDotsAroundIndex(item, index, length, false);
  blockPlaceholder.setBounds(getBoundsForPlaceholder(index, length));

  previousPlaceholderIndex = index;
  currentPlaceholderLength = length;
}

Rectangle<int> BlockGridComponent::getBoundsForPlaceholder(Index& index, int length) {
  auto position = pointForIndex(index);
  auto width = getModuleBounds().getWidth() * length + (length - 1) * config.spacing + config.edgeSpacing * 2;
  return Rectangle<int>(position.getX() - config.edgeSpacing,
    position.getY() - config.edgeSpacing,
    width,
    config.itemHeight + config.edgeSpacing * 2);
}

void BlockGridComponent::gridItemEndedDrag(GridItemComponent* item, const MouseEvent& event) {
  GridComponent::gridItemEndedDrag(item, event);
  blockPlaceholder.setVisible(false);
}

void BlockGridComponent::setItemLength(GridItemComponent* item, int length) {
  GridComponent::setItemLength(item, length);
  resetDots();
}

void BlockGridComponent::itemLandedOutside(GridItemComponent* item, Index index) {
  resetDots();
}

void BlockGridComponent::applyMouseHoverEffect() {
  const float threshold = 150;

  for (int i = 0; i < dots.size(); i++) {
    auto dotties = dots.getUnchecked(i);
    for (int j = 0; j < dotties->size(); j++) {
      auto dot = dotties->getUnchecked(j);
      auto pos = dot->getPosition();
      auto distance = pos.getDistanceFrom(getMouseXYRelative());
      if (distance <= threshold) {
        auto mappedDistance = jmap(float(distance), 0.f, threshold, 0.4f, 0.0f);
        dot->setColour(DotComponent::defaultColour.brighter(mappedDistance));
      }
    }
  }
}

void BlockGridComponent::clear() {
  GridComponent::clear();
  resetDots();
}

void BlockGridComponent::gridItemStartedDrag(GridItemComponent* item, const MouseEvent& mouseEvent) {
  GridComponent::gridItemStartedDrag(item, mouseEvent);
  auto block = dynamic_cast<BlockComponent*>(item);
  block->animate();
}

void BlockGridComponent::animateDragMode(GridItemComponent* item, bool enabled) {
  auto bounds = item->getBounds();

  auto animation = [bounds, item](float value, float progress) {
    auto width = bounds.getWidth() * value;
    auto height = bounds.getHeight() * value;
    auto x = bounds.getX() + (bounds.getWidth() - width) / 2;
    auto y = bounds.getY() + (bounds.getHeight() - height) / 2;
    item->setBounds(x, y, width, height);
  };

  float scaleAtDrag = 0.93f;

  auto completion = [bounds, item, scaleAtDrag]() {
    auto factor = item->isDragging ? scaleAtDrag : 1.0f;
    auto width = bounds.getWidth() * factor;
    auto height = bounds.getHeight() * factor;
    auto x = bounds.getX() + (bounds.getWidth() - width) / 2;
    auto y = bounds.getY() + (bounds.getHeight() - height) / 2;
    item->setBounds(x, y, width, height);
  };

  EasingAnimator::AnimateInput input = {
    .seconds = 0.06f,
    .completion = completion,
    .animation = animation,
    .range = { 1.0f, scaleAtDrag },
    .points = { 0.50f, 0.75f, 0.30f, 1.10f }
  };

  animator.animate(input);
}