#include "gui/controls/BoxSlider.h"
#include "gui/ThemeManager.h"

void BoxSlider::sliderValueChanged(Slider* slider) {
  valueLabel.setText(slider->getTextFromValue(slider->getValue()), dontSendNotification);
}

BoxSlider::~BoxSlider() {
  slider.setLookAndFeel(nullptr);
  ThemeManager::shared()->removeListener(this);
}

BoxSlider::BoxSlider() {
  setName("BoxSlider");

  modulation_indication_highlight_.setFill(juce::FillType(Colour(134, 118, 177)));
  addChildComponent(modulation_indication_highlight_);

  slider.setLookAndFeel(&lnf);
  slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
  slider_parent.addAndMakeVisible(slider);
  slider_parent.setMouseCursor(MouseCursor::PointingHandCursor);
  slider.setMouseCursor(MouseCursor::PointingHandCursor);
  slider_parent.setInterceptsMouseClicks(false, true);
  slider.addListener(this);
  slider.setName("blocks_core_slider"); // find a better name
  addAndMakeVisible(slider_parent);
  setupLabel();

  slider.addMouseListener(this, false);
  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());

  setupIndicationAnimator();

  // easing_animator_.callback = [this](float value) {
  // };

  modulation_selection_highlight_.setFill(juce::FillType(Colour(255, 222, 161)));
  addAndMakeVisible(modulation_selection_highlight_);
}

void BoxSlider::setupIndicationAnimator() {
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

void BoxSlider::resized() {
  BaseButton::resized();
  slider.setBounds(getContent()->getLocalBounds());
  valueLabel.setBounds(getLocalBounds());
  Path p;
  float corner_radius = getHeight() / 3.0f;
  p.addRoundedRectangle(getLocalBounds().toFloat(), corner_radius);
  modulation_indication_highlight_.setPath(p);
  modulation_selection_highlight_.setBounds(getBounds());
}

void BoxSlider::setupLabel() {
  addAndMakeVisible(valueLabel);
  valueLabel.setText("", dontSendNotification);
  valueLabel.setJustificationType(Justification::centred);
  valueLabel.setInterceptsMouseClicks(false, false);
  valueLabel.setColour(Label::ColourIds::textColourId, Colour(200, 200, 200));
  valueLabel.setFont(Font(13));
  // valueLabel.setName("asdf");
}

void BoxSlider::themeChanged(Theme theme) {
  valueLabel.setColour(Label::ColourIds::textColourId, theme.two.brighter(0.3f));
}

void BoxSlider::mouseDown(const MouseEvent& event) {
  auto isRightClick = event.mods.isRightButtonDown();
  if (isRightClick) {
    slider.setValue(default_value_, dontSendNotification);
  }
}

// void BoxSlider::enableModulationSelectionHighlight(bool shouldHighlight) { // tdo : better name
  // easing_n
// }

void BoxSlider::selectedCompletion() {
  BaseButton::selectedCompletion();
  slider.setBounds(getContent()->getLocalBounds());
}

void BoxSlider::deselectedCompletion() {
  BaseButton::deselectedCompletion();
  slider.setBounds(getContent()->getLocalBounds());
}

void BoxSlider::selectedAnimation(float value, float progress) {
  BaseButton::selectedAnimation(value, progress);
  slider.setBounds(getContent()->getLocalBounds());
}

void BoxSlider::deselectedAnimation(float value, float progress) {
  BaseButton::deselectedAnimation(value, progress);
  slider.setBounds(getContent()->getLocalBounds());
}

void BoxSlider::setIndicationHighlight(bool shouldHighlight, Colour color) {
  if (!modulatable) return;
  modulation_indication_highlight_.setVisible(shouldHighlight);
  modulation_indication_highlight_.setFill(juce::FillType(Colour(255, 237, 174)));
  modulation_indication_highlight_.setAlpha(0.2f);
  value_animator_.reset();
  if (shouldHighlight) {
    value_animator_.start();
  } else {
    value_animator_.stop();
  }
}

void BoxSlider::mouseEnter(const MouseEvent& event) {
  if (is_mouse_inside_) return;
  
  is_mouse_inside_ = true;
  BaseButton::mouseEnter(event);
  setMouseCursor(MouseCursor::PointingHandCursor);
}

void BoxSlider::mouseExit(const MouseEvent& event) {
  if (contains(event.getPosition())) return;
  is_mouse_inside_ = false;
  BaseButton::mouseExit(event);
}
