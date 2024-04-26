#include "gui/controls/BoxSlider.h"
#include "gui/ThemeManager.h"

void BlocksSlider::sliderValueChanged(Slider* slider) {
  float value = slider->getValue();
  value_label_.setText(slider->getTextFromValue(value), dontSendNotification);
  if (listener_) listener_->sliderAdjusted(this, value);
}

void BlocksSlider::sliderDragStarted(Slider* slider) {
  if (listener_)listener_->sliderGestureChanged(this, true);
}

void BlocksSlider::sliderDragEnded(Slider* slider) {
  if (listener_)listener_->sliderGestureChanged(this, false);
}

BlocksSlider::~BlocksSlider() {
  slider_.setLookAndFeel(nullptr);
  ThemeManager::shared()->removeListener(this);
}

BlocksSlider::BlocksSlider(Listener* listener): listener_(listener) {
  setName("BoxSlider");
  modulation_indication_highlight_.setFill(juce::FillType(Colour(134, 118, 177)));
  addChildComponent(modulation_indication_highlight_);

  setupSlider();
  setupSliderContainer();

  modulation_selection_highlight_.setFill(juce::FillType(Colour(255, 222, 161)));
  setupLabel();

  slider_.addMouseListener(this, false);
  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());

  setupIndicationAnimator();
}

void BlocksSlider::setupSliderContainer() {
  slider_container_.addAndMakeVisible(slider_);
  slider_container_.addChildComponent(modulation_selection_highlight_);
  slider_container_.setMouseCursor(MouseCursor::PointingHandCursor);
  slider_container_.setInterceptsMouseClicks(false, true);
  addAndMakeVisible(slider_container_);
}

void BlocksSlider::setupSlider() {
  slider_.setLookAndFeel(&lnf);
  slider_.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
  slider_.setMouseCursor(MouseCursor::PointingHandCursor);
  slider_.addListener(this);
  slider_.setName("blocks_core_slider"); // find a better name
}

void BlocksSlider::setupIndicationAnimator() {
  value_animator_.waveType = ValueAnimator::WaveType::triangle;
  value_animator_.repeating = true;
  value_animator_.speed = 1.3f;
  value_animator_.valueAnimatorCallback = [this](float value) {
    auto mapped = juce::jmap(value, 0.0f, 1.0f, 0.2f, 1.4f);
    modulation_indication_highlight_.setAlpha(mapped);
    repaint();
  };
}

// void BoxSlider::paint(juce::Graphics& g) {
  // g.setColour(Colours::red);
// }

void BlocksSlider::resized() {
  BaseButton::resized();
  slider_.setBounds(getContent()->getLocalBounds());
  value_label_.setBounds(getLocalBounds());
  float corner_radius = getHeight() / 3.25f;
  Path indication_path;
  indication_path.addRoundedRectangle(getLocalBounds().toFloat(), corner_radius);
  modulation_indication_highlight_.setPath(indication_path);
  resizeSelectionHighlight();
}

void BlocksSlider::resizeSelectionHighlight() {
  Path selection_path;
  selection_path.addRoundedRectangle(getContent()->getLocalBounds().reduced(1, 1).toFloat(), getHeight() / 4.00f);
  modulation_selection_highlight_.setPath(selection_path);
}

void BlocksSlider::setupLabel() {
  addAndMakeVisible(value_label_);
  value_label_.setText("", dontSendNotification);
  value_label_.setJustificationType(Justification::centred);
  value_label_.setInterceptsMouseClicks(false, false);
  value_label_.setColour(Label::ColourIds::textColourId, Colour(200, 200, 200));
  value_label_.setFont(Font(13));
}

void BlocksSlider::themeChanged(Theme theme) {
  value_label_.setColour(Label::ColourIds::textColourId, theme.two.brighter(0.4f));
}

void BlocksSlider::mouseDown(const MouseEvent& event) {
  auto isRightClick = event.mods.isRightButtonDown();
  if (isRightClick) {
    slider_.setValue(default_value_, dontSendNotification);
  }
}

void BlocksSlider::selectedCompletion() {
  BaseButton::selectedCompletion();
  slider_.setBounds(getContent()->getLocalBounds());
  resizeSelectionHighlight();
}

void BlocksSlider::deselectedCompletion() {
  BaseButton::deselectedCompletion();
  slider_.setBounds(getContent()->getLocalBounds());
  resizeSelectionHighlight();
}

void BlocksSlider::selectedAnimation(float value, float progress) {
  BaseButton::selectedAnimation(value, progress);
  slider_.setBounds(getContent()->getLocalBounds());
  resizeSelectionHighlight();
}

void BlocksSlider::deselectedAnimation(float value, float progress) {
  BaseButton::deselectedAnimation(value, progress);
  slider_.setBounds(getContent()->getLocalBounds());
  resizeSelectionHighlight();
}

void BlocksSlider::setIndicationHighlight(bool shouldHighlight, Colour color) {
  current = color.getPerceivedBrightness() > 0.5 ? Colours::black : Colours::white;
  if (!modulatable) return;
  modulation_indication_highlight_.setVisible(shouldHighlight);
  modulation_indication_highlight_.setFill(color);
  modulation_selection_highlight_.setFill(color);
  modulation_indication_highlight_.setAlpha(0.2f);
  value_animator_.reset();
  if (shouldHighlight) {
    value_animator_.start();
  } else {
    value_animator_.stop();
  }
}

void BlocksSlider::mouseEnter(const MouseEvent& event) {
  if (is_mouse_inside_) return;
  is_mouse_inside_ = true;
  BaseButton::mouseEnter(event);
  setMouseCursor(MouseCursor::PointingHandCursor);
}

void BlocksSlider::mouseExit(const MouseEvent& event) {
  if (contains(event.getPosition())) return;
  is_mouse_inside_ = false;
  BaseButton::mouseExit(event);
}

void BlocksSlider::startModulationSelectionAnimation() {
  modulation_selection_highlight_.setVisible(true);
  modulation_indication_highlight_.setVisible(false);
  value_label_.setColour(Label::ColourIds::textColourId, current);
  slider_.setAlpha(0.1f);
  startSelectedAnimation();
}

void BlocksSlider::stopModulationSelectionAnimation() {
  value_label_.setColour(Label::ColourIds::textColourId, ThemeManager::shared()->getCurrent().two.brighter(0.4f));
  modulation_selection_highlight_.setVisible(false);
  modulation_indication_highlight_.setVisible(true);
  slider_.setAlpha(1.0f);
  startDeselectedAnimation();
}