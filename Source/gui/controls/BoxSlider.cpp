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
  addAndMakeVisible(slider);
  slider.addListener(this);
  slider.setName("blocks_core_slider"); // find a better name
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

void BoxSlider::paint(juce::Graphics& g) {
  // g.setColour(Colours::red);
}

void BoxSlider::resized() {
  slider.setBounds(getLocalBounds());
  valueLabel.setBounds(getLocalBounds());
  // highlight.setBounds(getLocalBounds());
  Path p;
  float corner_radius = getHeight() / 3.0f;
  p.addRoundedRectangle(getLocalBounds().toFloat(), corner_radius);
  modulation_indication_highlight_.setPath(p);
  modulation_selection_highlight_.setBounds(getBounds());
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

// void BoxSlider::enableModulationSelectionHighlight(bool shouldHighlight) { // tdo : better name
  // easing_n
// }

void BoxSlider::setIndicationHighlight(bool shouldHighlight, Colour color) {
  if (!modulatable) return;
  modulation_indication_highlight_.setVisible(shouldHighlight);
  // drawable_path_.setFill(juce::FillType(Colour(255, 222, 161)));
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
  // setIndicationHighlight(true, Colour(255, 222, 161));
  auto expanded_bounds = getLocalBounds().expanded(2);
  std::cout << "expand: " << expanded_bounds.toString().toStdString() << std::endl;
  setBounds(expanded_bounds);
}

void BoxSlider::mouseExit(const MouseEvent& event) {
  auto reduced_bounds = getLocalBounds().reduced(2);
  std::cout << "expand: " << reduced_bounds.toString().toStdString() << std::endl;
  setBounds(reduced_bounds);
  // setIndicationHighlight(false, Colour(255, 222, 161));
  printf("exit\n");
}
