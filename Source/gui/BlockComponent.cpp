/*
 ==============================================================================

 BlockComponent.cpp
 Created: 29 May 2020 7:01:12pm
 Author:  dange

 ==============================================================================
 */


#include "gui/BlockComponent.h"
#include "model/ModuleParameter.h"
#include "gui/ThemeManager.h"
#include "BinaryData.h"

BlockComponent::~BlockComponent() {
  ThemeManager::shared()->removeListener(this);
}

OscillatorPainter* BlockComponent::getPainter() { return painter.get(); }

void BlockComponent::resizePainter() {
  if (painter == nullptr) return;
  float horizontalPadding = 14.0f;
  float width = getWidth() - horizontalPadding;
  float height = getHeight() / 2.60f;
  float x = horizontalPadding / 2.0f;
  float y = getHeight() / 2.0f - height / 2.0f;
  painter->setBounds(x, y, width, height);
}

BlockComponent::BlockComponent(Index index): GridItemComponent({ index, Constants::blockWidth }) {
  isSelected = false;
  isStretching = false;
  isDragging = false;
  listener = nullptr;

  setupTitleLabel();
  setBounds(0, 0, Constants::blockWidth, Constants::blockHeight);

  initialWidth = Constants::blockWidth;
  darkener.toFront(false);
  darkener.setInterceptsMouseClicks(false, false);

  indicators.setInterceptsMouseClicks(false, false);
  addAndMakeVisible(indicators);

  ThemeManager::shared()->addListener(this);
  setName("BlockComponent");
}

void BlockComponent::setupTitleLabel() {
  titleLabel.setJustificationType(Justification::bottomLeft);
  titleLabel.setFont(Font(13.0f));
  titleLabel.setBorderSize(BorderSize(0));
  titleLabel.setInterceptsMouseClicks(false, false);
  titleLabel.setColour(Label::ColourIds::textColourId, labelColour);

  addAndMakeVisible(titleLabel);
}

void BlockComponent::paint(Graphics& g) {
  g.reduceClipRegion(clipRegion);
  auto theme = ThemeManager::shared()->getCurrent();

  if (isSelected) {
    g.setColour(theme.three);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRadius);
  } else {
    drawOutline(g);
    g.setColour(backgroundColour);
    g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRadius);
  }

  if (isStretchable) {
    drawStretchIndicator(g);
  }
}

void BlockComponent::drawStretchIndicator(Graphics& g) {
  int startY = getHeight() / 3 - 1;
  g.setColour(selectionColour);

  for (int i = 0; i < 3; i++) {
    int y = startY + i * 4;
    g.fillEllipse(getWidth() - 8, y + i * 2, 3, 3);
  }
}

void BlockComponent::resized() {
  clipRegion.clear();
  clipRegion.addRoundedRectangle(0, 0, getWidth(), getHeight(), cornerRadius);
  titleLabel.setBounds(5, 1, getWidth(), getHeight() - 4);

  darkener.setBounds(getLocalBounds());
  darkener.darkener.setPath(clipRegion);
  resizePainter();
  resizeIndicators();
}

void BlockComponent::resizeIndicators() {
  auto width = getWidth() / 1.5f;
  auto height = getHeight() / 4.0f;
  indicators.setBounds(5.0f, 5.0f, width, height);
}

void BlockComponent::setSelected(bool isSelected) {
  GridItemComponent::setSelected(isSelected);

  auto theme = ThemeManager::shared()->getCurrent();
  setTitleColour(isSelected ? Colours::black : selectionColour);
  if (painter != nullptr) {
    painter->waveColour = isSelected ? theme.two : selectionColour;
  }
}

void BlockComponent::drawEnabled(Graphics& g) {
  g.setColour(this->isSelected ? backgroundColourSelected : backgroundColour);
  g.fillRoundedRectangle(getLocalBounds().toFloat(), cornerRadius);
}

void BlockComponent::drawOutline(Graphics& g) {
  Path path;
  path.addRoundedRectangle(getLocalBounds().toFloat(), cornerRadius);

  g.setColour(selectionColour);
  g.strokePath(path, PathStrokeType(4));
}

void BlockComponent::drawDisabled(Graphics& g) {
  Path path;
  path.addRoundedRectangle(getLocalBounds().toFloat(), cornerRadius);
  g.setColour(Colours::white.withAlpha(0.2f));
  g.strokePath(path, PathStrokeType(4));
}

void BlockComponent::remove(bool animated) {
  setTitleColour(Colours::transparentBlack);
  auto finalBounds = Rectangle<int>(getX() + getWidth() / 2, getY() + getHeight() / 2, 0, 0);
  Desktop::getInstance().getAnimator().animateComponent(this, finalBounds, 0, 75, true, 1, 1);
}

void BlockComponent::setPainter(OscillatorPainter* component) {
  if (component == nullptr) return;

  if (painter != nullptr) {
    removeChildComponent(painter.get());
    painter.reset();
  }

  painter = std::unique_ptr<OscillatorPainter>(component);
  addAndMakeVisible(painter.get(), 0);
  resizePainter();
}

void BlockComponent::setEnvelopePath(Colour colour) {
  envelopePath = std::make_unique<EnvelopePath>();
  addAndMakeVisible(envelopePath.get(), 0);
  envelopePath->setBounds(getBounds().reduced(6, 6));
  envelopePath->colour = colour;
}

BlockComponent* BlockComponent::create(std::shared_ptr<model::Block> block) {
  auto component = new BlockComponent(block->index);

  component->setTitle(block->display_name);
  component->isStretchable = block->category == model::Module::Category::effect;
  component->length = block->length;
  component->colour = block->colour.colour;

  if (block->id.type == Model::Types::osc) {
    float waveformFloat = block->parameters_[0]->value_processor->value();  
    int waveformInt = static_cast<int>(waveformFloat);
    auto painter = new OscillatorPainter();
    painter->setWaveformType(getWaveformType(waveformInt));
    painter->thickness = 2.0f;
    component->setPainter(painter);
  } else if (block->id.type == Model::Types::noise) {
    auto painter = new OscillatorPainter();
    painter->setWaveformType(OscillatorPainter::WaveformType::noise);
    painter->thickness = 2.0f;
    component->setPainter(painter);
  } 

  component->themeChanged(ThemeManager::shared()->getCurrent());
  return component;
}

void BlockComponent::themeChanged(Theme theme) {
  float delta = 0.6f;
  selectionColour = theme.dark ? theme.two.brighter(delta) : theme.two.darker(delta);
  titleLabel.setColour(Label::ColourIds::textColourId, selectionColour);

  if (painter != nullptr)
    painter->waveColour = selectionColour;
}

void BlockComponent::setConfig(std::shared_ptr<model::Module> m, std::vector<std::shared_ptr<model::Connection>> connections) {
  indicators.reset();

  std::unordered_set<std::shared_ptr<model::Module>> uniqueSources;
  std::vector<std::shared_ptr<model::Module>> uniqueSourceVector;

  for (const auto& connection : connections) { 
    if (connection->target == m) {
      uniqueSources.insert(connection->source);
    }
  }

  // for (const auto& param : m->parameters_)
  //   for (const auto& connection : param->connections)
  //     uniqueSources.insert(connection->source);

  uniqueSourceVector.assign(uniqueSources.begin(), uniqueSources.end());

  for (auto source : uniqueSourceVector) {
    indicators.addModulationIndicator(source->colour.colour);
  }
}

void BlockComponent::animate() {
  auto bounds = getBounds();

  auto animation = [bounds, this](float value, float progress) {
    auto width = bounds.getWidth() * value;
    auto height = bounds.getHeight() * value;
    auto x = bounds.getX() + (bounds.getWidth() - width) / 2;
    auto y = bounds.getY() + (bounds.getHeight() - height) / 2;
    setBounds(x, y, width, height);
  };

  const float scaleAtDrag = 0.93f;

  auto completion = [bounds, scaleAtDrag, this]() {
    auto factor = isDragging ? scaleAtDrag : 1.0f;
    auto width = bounds.getWidth() * factor;
    auto height = bounds.getHeight() * factor;
    auto x = bounds.getX() + (bounds.getWidth() - width) / 2;
    auto y = bounds.getY() + (bounds.getHeight() - height) / 2;
    setBounds(x, y, width, height);
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