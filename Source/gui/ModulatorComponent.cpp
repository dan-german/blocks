/*
  ==============================================================================

    ModulatorComponent.cpp
    Created: 29 Oct 2021 2:44:00am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/ModulatorComponent.h"

void ModulatorComponent::sliderDragEnded(Slider* slider) {
  delegate_->modulatorGestureChanged(this, slider_parameter_name_map_[slider], false);
}

ModulatorComponent::~ModulatorComponent() {
  sliders.clear(true);
  ThemeManager::shared()->removeListener(this);
}

ModulatorComponent::ModulatorComponent(BlocksSlider::Listener* listener): slider_container_(listener, gui::SliderContainer::modulator_config) {
  addAndMakeVisible(drag_indicator_);

  addAndMakeVisible(drag_hitbox_);
  drag_hitbox_.setName(ModulatorComponent::dragComponentIdentifier);
  drag_hitbox_.addMouseListener(this, false);

  addChildComponent(envelopePath);
  addChildComponent(oscillatorPainter);
  oscillatorPainter.setWaveformType(OscillatorPainter::WaveformType::sine);
  oscillatorPainter.speed = 15.0f;
  oscillatorPainter.cycles = 2.5f;

  setupRemoveButton();
  addAndMakeVisible(slider_container_);
  setupSliders();
  setupTitle();

  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());
}

void ModulatorComponent::setupRemoveButton() {
  addAndMakeVisible(exitButton);
  exitButton.onClick = [this]() { delegate_->modulatorRemoved(this); };
}

void ModulatorComponent::setupSliders() {
  for (int i = 0; i < 4; i++) {
    auto slider = new LabeledSlider(blocks_slider_listener);
    sliders.add(slider);
    // addAndMakeVisible(slidersContainer);
    slidersContainer.addAndMakeVisible(slider);
    slider->box_slider_.juce_slider_.addListener(this);
  }
}

void ModulatorComponent::setupTitle() {
  addAndMakeVisible(title);
  title.setText("lfo 1", dontSendNotification);
  title.setJustificationType(Justification::right);
  title.setColour(Label::ColourIds::textColourId, ThemeManager::shared()->getCurrent().two);
}

void ModulatorComponent::paint(Graphics& g) {
  drawBottomLine(g);
  // g.fillAll(Colours::blueviolet);
}

void ModulatorComponent::drawBottomLine(Graphics& g) const {
  g.setColour(ThemeManager::shared()->getCurrent().one.withAlpha(0.5f));
  float horizontalInsets = 13.0f;
  float y = getHeight() - 2.0f;
  g.fillRect(horizontalInsets, y, getWidth() - horizontalInsets * 2.0f, 2.0f);
}

void ModulatorComponent::resized() {
  bounds = getLocalBounds();
  std::cout << "modulator bounds " << bounds.toString() << std::endl;

  resizeDragIndicator();
  resizeSliderContainer();
  resizeSliders();
  resizeExitButton();
  resizeTitle();
  resizeOscillatorPainter();
  resizeEnvelopePath();
}

void ModulatorComponent::resizeExitButton() {
  auto size = 11;
  auto x = bounds.getX() + bounds.getWidth() - size - 17;
  int y = drag_indicator_.getY() + drag_indicator_.getHeight() / 2 - 6;
  exitButton.setBounds(x, y, size, size);
}

void ModulatorComponent::resizeOscillatorPainter() {
  int x = drag_indicator_.getX() + drag_indicator_.getWidth() + 12;
  int width = 50;
  int height = 16;
  int y = drag_indicator_.getBounds().getCentreY() - height / 2 - 1;
  oscillatorPainter.setBounds(x, y, width, height);
}

void ModulatorComponent::resizeEnvelopePath() {
  int x = drag_indicator_.getX() + drag_indicator_.getWidth() + 12;
  int y = drag_indicator_.getY() + 2;
  int width = 55;
  int height = 14;
  envelopePath.setBounds(x, y, width, height);
}

void ModulatorComponent::resizeDragIndicator() {
  int width = 13;
  auto indicatorBounds = Rectangle<int>(bounds.getX() + topSpacing, bounds.getY() + topSpacing, width, dragIndicatorHeight);
  drag_indicator_.setBounds(indicatorBounds);
  drag_hitbox_.setBounds(
    indicatorBounds.getX() - 6,
    indicatorBounds.getY() - 6,
    indicatorBounds.getWidth() + 10,
    indicatorBounds.getHeight() + 9);
}

void ModulatorComponent::resizeSliderContainer() {
  int yOffset = drag_indicator_.getY() + drag_indicator_.getHeight() + dragIndicatorBottomSpacing;
  int rows = static_cast<int>(ceil(sliders.size() / float(columns)));

  slidersContainer.setBounds(bounds.getX() + sliderHorizontalInsets,
    bounds.getY() + yOffset,
    bounds.getWidth() - sliderHorizontalInsets * 2,
    rows * sliderHeight + (rows - 1) * sliderSpacing);
}

void ModulatorComponent::resizeTitle() {
  int width = 50;
  int height = 20;
  int y = drag_indicator_.getY() + drag_indicator_.getHeight() / 2;
  title.setBounds(exitButton.getX() - width - 2, y - height / 2 - 1, width, height);
}

void ModulatorComponent::resizeSliders() {
  int yOffset = drag_indicator_.getY() + drag_indicator_.getHeight() + dragIndicatorBottomSpacing;
  int height_left = getHeight() - yOffset;

  int horizontal_insets = 4;
  slider_container_.config_.slider_width = getWidth() / 2 - horizontal_insets * 2; // 4 on both sides
  slider_container_.setBounds(horizontal_insets, yOffset, getWidth(), height_left);
  // slidr

  // slider_container_.setBounds(0, 0, 20, 20);

  // for (int row = 0; row < rows; row++) {
  //   for (int column = 0; column < columns; column++) {
  //     int index2 = row + column + acc;

  //     printf("%d %d\n", pls, row + column + acc);
  //     pls++;
  //     auto slider = sliders[row + column + acc];
  //     int x = column * sliderWidth + sliderHorizontalSpacing;
  //     int y = row * sliderHeight + sliderSpacing * row;
  //     slider->setBounds(x, y, sliderWidth - sliderHorizontalSpacing * 2, sliderHeight);
  //   }
  //   acc++;
  // }

  // for (int i = 0; i < remainder; i++) {
  //   auto slider = sliders[count - remainder + i];
  //   int x = i * sliderWidth;

  //   for (int column = 0; column < columns; column++) {
  //     int y = rows * sliderHeight + yOffset + sliderSpacing * column;
  //     slider->setBounds(x, y, sliderWidth, sliderHeight);
  //   }
  // }
}

int ModulatorComponent::calculateHeight() {
  int numberOfSliders = 4;
  int rows = static_cast<int>(std::ceil(numberOfSliders / float(columns)));
  int yOffset = topSpacing + dragIndicatorHeight + dragIndicatorBottomSpacing;
  int bottomSpacing = 10;
  return yOffset + rows * sliderHeight + (rows - 1) * sliderSpacing + bottomSpacing;
}

void ModulatorComponent::mouseDown(const MouseEvent& event) {
  if (event.eventComponent->getName() == ModulatorComponent::dragComponentIdentifier) {
    drag_hitbox_.setMouseCursor(MouseCursor::NoCursor);
    delegate_->modulatorStartedDrag(this, event);
  }
}

void ModulatorComponent::mouseUp(const MouseEvent& event) {
  drag_hitbox_.setMouseCursor(MouseCursor::NormalCursor);

  if (event.eventComponent->getName() == ModulatorComponent::dragComponentIdentifier)
    delegate_->modulatorEndedDrag(this, event);
}

void ModulatorComponent::mouseEnter(const MouseEvent& event) {
  if (event.eventComponent->getName() == ModulatorComponent::dragComponentIdentifier)
    drag_hitbox_.setMouseCursor(MouseCursor::DraggingHandCursor);
}

void ModulatorComponent::mouseExit(const MouseEvent& event) {
  if (event.eventComponent->getName() == ModulatorComponent::dragComponentIdentifier)
    drag_hitbox_.setMouseCursor(MouseCursor::NormalCursor);
}

void ModulatorComponent::mouseDrag(const MouseEvent& event) {
  if (event.eventComponent->getName() == ModulatorComponent::dragComponentIdentifier)
    delegate_->modulatorIsDragging(this, event);
}

void ModulatorComponent::setColour(Colour colour) {
  this->colour = colour;
  drag_indicator_.colour = colour;
  drag_indicator_.repaint();
  title.setColour(Label::ColourIds::textColourId, colour);
  oscillatorPainter.waveColour = colour;
  envelopePath.colour = colour;
  slider_container_.setSlidersColour(colour);
}

void ModulatorComponent::sliderDragStarted(Slider* slider) {
  for (int i = 0; i < sliders.size(); i++)
    if (&sliders[i]->box_slider_.juce_slider_ == slider) {
      currentSliderIndex = i;
      break;
    }
  delegate_->modulatorGestureChanged(this, slider_parameter_name_map_[slider], true);
}

void ModulatorComponent::sliderValueChanged(Slider* slider) {
  float value = static_cast<float>(slider->getValue());
  // if (onSliderValueChange) onSliderValueChange(currentSliderIndex, value);
  auto name = slider_parameter_name_map_[slider];
  delegate_->modulatorIsAdjusting(this, name, value);
}

void ModulatorComponent::themeChanged(Theme theme) {
  title.setColour(Label::ColourIds::textColourId, theme.two);
  repaint();
}