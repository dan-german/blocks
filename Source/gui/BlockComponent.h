/*
  ==============================================================================

  BlockComponent.h
  Created: 29 May 2020 7:01:12pm
  Author:  dange

  ==============================================================================
*/

#pragma once

#include "gui/OutlinedLabel.h"
#include "gui/GridItemComponent.h"
#include "gui/OscillatorPainter.h"
#include "gui/EasingAnimator.h"
#include "gui/EnvelopePath.h"
#include "model/Block.h"
#include "gui/ThemeListener.h"
#include "gui/CircleIndicatorList.h"
#include "gui/CircleIndicators.h"
#include "module_new.h"

class BlockComponent;
using Block = Model::Block;
using Module = Model::Module;

class BlockComponent final: public GridItemComponent, ThemeListener {
public:
  Colour backgroundColour = Colours::black.withAlpha(0.1f);
  Colour backgroundColourSelected = Colour(170, 170, 170).withAlpha(0.25f);
  Colour labelColour = Colour(180, 180, 180);
  Colour selectionColour = Colour(95, 95, 95);
  CircleIndicatorList circleIndicatorList;
  CircleIndicators indicators;

  int initialWidth;
  float cornerRadius = 8.0f;
  Colour colour;

  BlockComponent(Index index);
  ~BlockComponent() override;

  OscillatorPainter* getPainter();
  void setEnvelopePath(Colour colour);
  void setSelected(bool isSelected) override;
  void remove(bool animated);
  void setPainter(OscillatorPainter* component);

  void setTitle(String text) { this->titleLabel.setText(text, dontSendNotification); }
  void setTitleColour(Colour newColour) { this->titleLabel.setColour(Label::ColourIds::textColourId, newColour); }
  EnvelopePath* getEnvelopePath() { return this->envelopePath.get(); }

  static BlockComponent* create(std::shared_ptr<model::Block> block);
  void setConfig(std::shared_ptr<Module> m);
  void animate();
protected:
  void paint(Graphics& g) override;
  void resized() override;
private:
  std::unique_ptr<EnvelopePath> envelopePath;
  EasingAnimator animator;
  Label titleLabel;
  Path clipRegion;
  float selectionAlpha = 0.0f;
  std::unique_ptr<OscillatorPainter> painter;
  bool isPlaceholder = false;

  void drawStretchIndicator(Graphics& g);
  void drawEnabled(Graphics& g);
  void drawDisabled(Graphics& g);
  void drawOutline(Graphics& g);
  void resizePainter();
  void setupTitleLabel();
  void themeChanged(Theme theme) override;
  void resizeIndicators();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BlockComponent)
};