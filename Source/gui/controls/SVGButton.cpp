#include "gui/controls/SVGButton.h"
#include "BinaryData.h"

SVGButton::SVGButton() { }

void SVGButton::setSVG(const void* svgData, const size_t svgDataNumBytes) {
  const auto svg = Drawable::createFromImageData(svgData, svgDataNumBytes);
  auto button = new DrawableButton("myButton", DrawableButton::ButtonStyle::ImageFitted);

  button->setImages(svg.get());

  drawableButton.reset(button);
  drawableButton->setInterceptsMouseClicks(false, false);
  addAndMakeVisible(*drawableButton);

  setButtonColour(colour);

  drawableButton->onClick = [this]() { if (on_click_) on_click_(); };
}

void SVGButton::setButtonColour(Colour colour) {
  drawableButton->getNormalImage()->replaceColour(_currentColor, colour);
  _currentColor = colour;
}

void SVGButton::paint(juce::Graphics& g) {
  BaseButton::paint(g);
  setButtonColour(colour);
}

Component* SVGButton::getContent() { return drawableButton.get(); }