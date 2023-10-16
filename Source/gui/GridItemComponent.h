/*
  ==============================================================================

    GridItemComponent.h
    Created: 15 Oct 2020 7:58:14pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "model/Index.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "DarkBackground.h"

using namespace juce;
class GridComponent;

class GridItemComponent: public juce::Component {
public:
  struct Config {
    Index index;
    int width;
  };

  int length = 1;

  bool isStretching = false;
  bool isDraggable = true;
  bool isDragging = false;
  bool isSelected = false;
  bool isStretchable = true;
  bool isHidden = false;

  struct Listener;
  Listener* listener;

  GridItemComponent(Config config);
  DarkBackground darkener;
  Config config;
  Index index;
  ComponentDragger dragger;
  GridComponent* grid;

  virtual void setSelected(bool isSelected) {
    this->isSelected = isSelected;
    repaint();
  }

  virtual void setHidden(bool isHidden) {
    this->isHidden = isHidden;
    darkener.setVisible(isHidden);
  }

protected:
  void mouseDown(const MouseEvent& event) override;
  void mouseDrag(const MouseEvent& event) override;
  void mouseUp(const MouseEvent& event) override;
  void mouseMove(const MouseEvent& event) override;
  virtual ComponentBoundsConstrainer* getBoundsConstrainer() { return nullptr; }
};

struct GridItemComponent::Listener {
protected:
  ~Listener() = default;
public:
  virtual void gridItemStartedDrag(GridItemComponent* item, const MouseEvent& event) = 0;
  virtual void gridItemEndedDrag(GridItemComponent* item, const MouseEvent& event) = 0;
  virtual void gridItemIsStretching(GridItemComponent* item, int offset) = 0;
  virtual void gridItemStretchEnded(GridItemComponent* item, int offset) = 0;
  virtual void gridItemClicked(GridItemComponent* item, const MouseEvent& event) = 0;
  virtual void gridItemIsDragging(GridItemComponent* item, const MouseEvent& event) = 0;
};