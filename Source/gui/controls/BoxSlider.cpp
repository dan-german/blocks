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
  addChildComponent(drawable_path_);
  slider.setLookAndFeel(&lnf);
  slider.setTextBoxStyle(Slider::NoTextBox, false, 0, 0);
  addAndMakeVisible(slider);
  slider.addListener(this);
  slider.setName("blocks_core_slider"); // find a better name
  setupLabel();
  slider.addMouseListener(this, false);
  ThemeManager::shared()->addListener(this);
  themeChanged(ThemeManager::shared()->getCurrent());

  drawable_path_.setFill(juce::FillType(Colour(134, 118, 177)));
  animator_.waveType = ValueAnimator::WaveType::triangle;
  animator_.repeating = true;
  animator_.speed = 1.3f;
  animator_.valueAnimatorCallback = [this](float value) {
    auto mapped = juce::jmap(value, 0.0f, 1.0f, 0.2f, 1.0f);
    drawable_path_.setAlpha(mapped);
    repaint();
  };
  animator_.start();
}

void BoxSlider::paint(juce::Graphics& g) {
  // g.setColour(Colours::red);
}

void BoxSlider::resized() {
  slider.setBounds(getLocalBounds());
  valueLabel.setBounds(getLocalBounds());
  // highlight.setBounds(getLocalBounds());
  Path p;
  float corner_radius = getHeight() / 3.5f;
  p.addRoundedRectangle(getLocalBounds().toFloat(), corner_radius);
  drawable_path_.setPath(p);
  // drawh
}

void BoxSlider::setupLabel() {
  addAndMakeVisible(valueLabel);
  valueLabel.setText("", dontSendNotification);
  valueLabel.setJustificationType(Justification::centred);
  valueLabel.setInterceptsMouseClicks(false, false);
  valueLabel.setColour(Label::ColourIds::textColourId, Colour(200, 200, 200));
  valueLabel.setFont(Font(13));
  valueLabel.setName("asdf");
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

void BoxSlider::highlight(bool shouldHighlight, Colour color) {
  if (!modulatable) return;
  drawable_path_.setVisible(shouldHighlight);
  drawable_path_.setFill(juce::FillType(color));
  animator_.reset();
  if (shouldHighlight) {
    animator_.start();
  } else {
    animator_.stop();
    // drawable_path_.setAlpha(1.0f);
  }
  // highlight
  // highlight.setVisible(shouldHighlight);
}

// void BaseButton::startSelectedAnimation() {
//   if (getContent() == nullptr) return;

//   EasingAnimator::AnimateInput input;
//   input.animation = [this](float value, float progress) { selectedAnimation(value, progress); };
//   input.completion = [this]() { selectedCompletion(); };

//   float rangeValues[] = { 0.95f, 1.0f };
//   std::copy(std::begin(rangeValues), std::end(rangeValues), std::begin(input.range));

//   float pointsValues[] = { 0.50f, 0.75f, 0.30f, 1.20f };
//   std::copy(std::begin(pointsValues), std::end(pointsValues), std::begin(input.points));

//   input.seconds = 0.06f;

//   animator.animate(input);

