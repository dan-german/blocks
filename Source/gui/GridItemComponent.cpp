/*
  ==============================================================================

    SnappableComponent.cpp
    Created: 15 Oct 2020 7:58:14pm
    Author:  Dan German

  ==============================================================================
*/

#include "gui/GridItemComponent.h"

GridItemComponent::GridItemComponent(Config config): config(config), index(config.index) {
  addChildComponent(darkener);
}

void GridItemComponent::mouseDown(const MouseEvent& event) {
  if (isStretchable) {
    const bool mouseIsInRightStretchArea = event.getPosition().getX() >= getWidth() - (0.20f * config.width);
    if (mouseIsInRightStretchArea) {
      setMouseCursor(MouseCursor::LeftRightResizeCursor);
      isStretching = true;
    }
  }

  dragger.startDraggingComponent(this, event);
}

void GridItemComponent::mouseDrag(const MouseEvent& event) {
  bool weakDrag = event.getDistanceFromDragStart() < 2;
  if (weakDrag) return;

  if (!isDraggable) return;

  if (isStretching) {
    listener->gridItemIsStretching(this, event.getPosition().getX() - config.width);
    return;
  }

  if (!isDragging) {
    isDragging = true;
    listener->gridItemStartedDrag(this, event);
  }

  listener->gridItemIsDragging(this, event);
  dragger.dragComponent(this, event, getBoundsConstrainer());
  setMouseCursor(MouseCursor::DraggingHandCursor);
}

void GridItemComponent::mouseUp(const MouseEvent& event) {
  setMouseCursor(MouseCursor::NormalCursor);

  if (isStretching) {
    listener->gridItemStretchEnded(this, event.getPosition().getX());
    isStretching = false;
  } else if (isDragging) {
    isDragging = false;
    listener->gridItemEndedDrag(this, event);
  } else {
    listener->gridItemClicked(this, event);
  }
}

void GridItemComponent::mouseMove(const MouseEvent& event) {
  const bool mouseIsInRightStretchArea = event.getPosition().getX() >= getWidth() - (0.20f * config.width);

  if (isStretchable && mouseIsInRightStretchArea) {
    setMouseCursor(MouseCursor::LeftRightResizeCursor);
  } else {
    setMouseCursor(MouseCursor::NormalCursor);
  }
}