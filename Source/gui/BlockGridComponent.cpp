#include "gui/BlockGridComponent.h"
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#include "ThemeManager.h"

BlockGridComponent::BlockGridComponent(GridComponent::Config config): GridComponent(config) {
  addAndMakeVisible(blockPlaceholder);
  spawnDots();

  dots_animator_.speed = 0.5f;
  dots_animator_.waveType = ValueAnimator::WaveType::was;
  dots_animator_.valueAnimatorCallback = [this](float value) {
    for (int column : highlighted_columns_) {
      auto offset = 0.0f;
      for (auto dot_row : dot_matrix_) {
        float sine_value = std::sin(value * M_PI * 2 + offset);
        offset -= M_PI * 0.8f / (float)dot_row->size();
        auto left_dot = dot_row->getUnchecked(column);
        auto right_dot = dot_row->getUnchecked(column + 1);
        auto mapped = jmap(sine_value, -1.0f, 1.0f, 0.0f, 2.0f);
        auto color = ThemeManager::shared()->getCurrent().two.brighter(3.5f).darker(mapped);
        left_dot->setColour(color);
        right_dot->setColour(color);
      }
    }
  };

  dots_animator_.start();
  ThemeManager::shared()->addListener(this);
}

void BlockGridComponent::highlightColumn(int start, int end) {
  highlight = true;
  this->firstHighlightedColumn = start;
  this->lastHighlightedColumn = end;

  hideAllItems(true);

  for (int column = start; column < end; column++) {
    for (int row = 0; row < config.rows; row++) {
      setItemHidden({ row, column }, false);
    }
  }
}

void BlockGridComponent::reset() {
  for (auto dotArray : dot_matrix_)
    for (auto dot : *dotArray)
      dot->setColour(ThemeManager::shared()->getCurrent().two);

  hideAllItems(false);
  highlight = false;
}

void BlockGridComponent::setDownFlowingHighlight(int column, bool active) {
  for (auto row : dot_matrix_) {
    for (auto dot : *row) {
      dot->setColour(ThemeManager::shared()->getCurrent().two);
    }
  }

  if (active) {
    highlighted_columns_.insert(column);
  } else {
    highlighted_columns_.erase(column);
  }
}

void BlockGridComponent::spawnDots() {
  int size = Constants::gridDotSize;

  for (int column = 0; column <= config.columns; column++) {
    dot_matrix_.add(new OwnedArray<DotComponent>());
    positions.add(Array<Point<int>>());
    for (int row = 0; row <= config.rows; row++) {
      auto dot = new DotComponent();
      dot_matrix_[column]->add(dot);
      addAndMakeVisible(dot);

      auto point = pointForIndex(Index(column, row));

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

void BlockGridComponent::resetDotsVisibility() {
  for (auto row : dot_matrix_) {
    for (auto dot : *row) {
      dot->setVisible(true);
    }
  }

  for (auto item : items) {
    if (item->length <= 1) continue;

    for (auto row : dot_matrix_) {
      for (auto dot : *row) {
        dot->setVisible(true);
      }
    }

    for (int column = 1; column < item->length; column++) {
      for (int row = 0; row <= 1; row++) {
        auto dot = dot_matrix_.getUnchecked(item->index.column + column)->getUnchecked(item->index.row + row);
        dot->setVisible(false);
      }
    }
  }
}

void BlockGridComponent::snapItem(GridItemComponent* item, Index index, bool resetBounds) {
  GridComponent::snapItem(item, index, resetBounds);
  resetDotsVisibility();
  auto block = dynamic_cast<BlockComponent*>(item);
  block->animate();
}

void BlockGridComponent::resizeDots() {
  for (int column = 0; column <= config.columns; column++) {
    for (int row = 0; row <= config.rows; row++) {
      auto dot = dot_matrix_.getUnchecked(column)->getUnchecked(row);
      auto point = pointForIndex(Index(column, row));

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
    for (int row = index.row; row <= index.row + 1; row++)
      dot_matrix_.getUnchecked(column)->getUnchecked(row)->setVisible(visible);

  for (auto block : items) {
    if (block->length <= 1 || block == blockComponent) continue;

    for (int column = 1; column < block->length; column++) {
      for (int row = 0; row <= 1; row++) {
        auto dot = dot_matrix_.getUnchecked(block->index.column + column)->getUnchecked(block->index.row + row);
        dot->setVisible(false);
      }
    }
  }
}

void BlockGridComponent::itemHovered(GridItemComponent* item, bool valid, bool inside, int proposedLength, Index index) {
  resetDotsVisibility();
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
  resetDotsVisibility();
}

void BlockGridComponent::clear() {
  GridComponent::clear();
  resetDotsVisibility();
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

void BlockGridComponent::themeChanged(Theme theme) {
  for (auto row : dot_matrix_)
    for (auto dot : *row)
      dot->setColour(theme.two);
}