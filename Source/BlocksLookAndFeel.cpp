/*
  ==============================================================================

  BlocksLookAndFeel.cpp
  Created: 19 Sep 2020 9:36:58pm
  Author:  dange

  ==============================================================================
*/

#include "BlocksLookAndFeel.h"
#include "Constants.h"
#include "ThemeManager.h"
#include "BinaryData.h"

BlocksLookAndFeel::BlocksLookAndFeel() {
  auto typeface = Typeface::createSystemTypefaceFor(BinaryData::RobotoMedium_ttf, BinaryData::RobotoMedium_ttfSize);
  LookAndFeel::getDefaultLookAndFeel().setDefaultSansSerifTypeface(typeface);
}

void BlocksLookAndFeel::drawLinearSlider(Graphics& g,
  int x,
  int y,
  int width,
  int height,
  float sliderPos,
  float minSliderPos,
  float maxSliderPos,
  const Slider::SliderStyle style,
  Slider& slider) {
  auto isTwoVal = (style == Slider::SliderStyle::TwoValueVertical || style == Slider::SliderStyle::TwoValueHorizontal);
  auto isThreeVal = (style == Slider::SliderStyle::ThreeValueVertical || style == Slider::SliderStyle::ThreeValueHorizontal);

  auto trackWidth = 10.0f;

  Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
    slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

  Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
    slider.isHorizontal() ? startPoint.y : (float)y);

  Path backgroundTrack;
  backgroundTrack.startNewSubPath(startPoint);
  backgroundTrack.lineTo(endPoint);
  // g.setColour(slider.findColour(Slider::backgroundColourId));
  auto theme = ThemeManager::shared()->getCurrent();
  g.setColour(theme.dark ? theme.one : theme.two);
  g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  Path valueTrack;
  Point<float> minPoint, maxPoint, thumbPoint;

  if (isTwoVal || isThreeVal) {
    minPoint = { slider.isHorizontal() ? minSliderPos : (float)width * 0.5f,
        slider.isHorizontal() ? (float)height * 0.5f : minSliderPos };

    if (isThreeVal)
      thumbPoint = { slider.isHorizontal() ? sliderPos : (float)width * 0.5f,
          slider.isHorizontal() ? (float)height * 0.5f : sliderPos };

    maxPoint = { slider.isHorizontal() ? maxSliderPos : (float)width * 0.5f,
        slider.isHorizontal() ? (float)height * 0.5f : maxSliderPos };
  } else if (slider.getProperties()["isCenter"].equals(true)) { // todo make vertical also pls
    auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    minPoint = startPoint;
    maxPoint = { kx, ky };

    if (sliderPos >= (float)x + (float)width * 0.5f) {
      minPoint = { (float)x + (float)width * 0.5f, ky };
      maxPoint = { kx, ky };
    } else {
      minPoint = { kx, ky };
      maxPoint = { ((float)x + (float)width * 0.5f), ky };
    }
  } else {
    auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;

    minPoint = startPoint;
    maxPoint = { kx, ky };
  }

  valueTrack.startNewSubPath(minPoint);
  valueTrack.lineTo(isThreeVal ? thumbPoint : maxPoint);

  if (slider.getProperties()["disabled"]) return;

  g.setColour(theme.three);
  g.strokePath(valueTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });

  if (!isTwoVal) {
    // g.setColour(theme.three);
    // g.fillEllipse(Rectangle<float>(static_cast<float> (trackWidth),
    //   static_cast<float> (trackWidth)).withCentre(isThreeVal ? thumbPoint : maxPoint));
  }

  if (isTwoVal || isThreeVal) {
    auto sr = jmin(trackWidth, (slider.isHorizontal() ? (float)height : (float)width) * 0.4f);
    auto pointerColour = slider.findColour(Slider::thumbColourId);

    if (slider.isHorizontal()) {
      drawPointer(g, minSliderPos - sr,
        jmax(0.0f, (float)y + (float)height * 0.5f - trackWidth * 2.0f),
        trackWidth * 2.0f, pointerColour, 2);

      drawPointer(g, maxSliderPos - trackWidth,
        jmin((float)(y + height) - trackWidth * 2.0f, (float)y + (float)height * 0.5f),
        trackWidth * 2.0f, pointerColour, 4);
    } else {
      drawPointer(g, jmax(0.0f, (float)x + (float)width * 0.5f - trackWidth * 2.0f),
        minSliderPos - trackWidth,
        trackWidth * 2.0f, pointerColour, 1);

      drawPointer(g, jmin((float)(x + width) - trackWidth * 2.0f, (float)x + (float)width * 0.5f), maxSliderPos - sr,
        trackWidth * 2.0f, pointerColour, 3);
    }
  }
}

void BlocksLookAndFeel::drawButtonText(Graphics& graphics, TextButton& button, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
  LookAndFeel_V2::drawButtonText(graphics, button, shouldDrawButtonAsHighlighted, shouldDrawButtonAsDown);
}

Font BlocksLookAndFeel::getTextButtonFont(TextButton& button, int buttonHeight) { return { jmin(16.0f, (float)buttonHeight * 0.6f) }; }

Slider::SliderLayout BlocksLookAndFeel::getSliderLayout(Slider& slider) {
  // 1. compute the actually visible textBox size from the slider textBox size and some additional constraints
  int minXSpace = 0;
  int minYSpace = 0;

  auto textBoxPos = slider.getTextBoxPosition();

  if (textBoxPos == Slider::TextBoxLeft || textBoxPos == Slider::TextBoxRight)
    minXSpace = 30;
  else
    minYSpace = 15;

  auto localBounds = slider.getLocalBounds();

  //    auto textBoxWidth  = jmax (0, jmin (slider.getTextBoxWidth(),  localBounds.getWidth() - minXSpace));
  //    auto textBoxHeight = jmax (0, jmin (slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));

  auto textBoxWidth = 60;
  auto textBoxHeight = jmax(0, jmin(slider.getTextBoxHeight(), localBounds.getHeight() - minYSpace));


  Slider::SliderLayout layout;

  // 2. set the textBox bounds

  if (textBoxPos != Slider::NoTextBox) {
    if (slider.isBar()) {
      layout.textBoxBounds = localBounds;
    } else {
      layout.textBoxBounds.setWidth(textBoxWidth);
      layout.textBoxBounds.setHeight(textBoxHeight);

      if (textBoxPos == Slider::TextBoxLeft) layout.textBoxBounds.setX(0);
      else if (textBoxPos == Slider::TextBoxRight) layout.textBoxBounds.setX(localBounds.getWidth() - textBoxWidth);
      else /* above or below -> centre horizontally */ layout.textBoxBounds.setX((localBounds.getWidth() - textBoxWidth) / 2);

      if (textBoxPos == Slider::TextBoxAbove) layout.textBoxBounds.setY(0);
      else if (textBoxPos == Slider::TextBoxBelow) layout.textBoxBounds.setY(localBounds.getHeight() - textBoxHeight);
      else /* left or right -> centre vertically */    layout.textBoxBounds.setY((localBounds.getHeight() - textBoxHeight) / 2);
    }
  }

  // 3. set the slider bounds

  layout.sliderBounds = localBounds;

  if (slider.isBar()) {
    layout.sliderBounds.reduce(1, 1);   // bar border
  } else {
    if (textBoxPos == Slider::TextBoxLeft) layout.sliderBounds.removeFromLeft(textBoxWidth);
    else if (textBoxPos == Slider::TextBoxRight) layout.sliderBounds.removeFromRight(textBoxWidth);
    else if (textBoxPos == Slider::TextBoxAbove) layout.sliderBounds.removeFromTop(textBoxHeight);
    else if (textBoxPos == Slider::TextBoxBelow) layout.sliderBounds.removeFromBottom(textBoxHeight);

    const int thumbIndent = getSliderThumbRadius(slider);

    if (slider.isHorizontal()) layout.sliderBounds.reduce(thumbIndent, 0);
    else if (slider.isVertical()) layout.sliderBounds.reduce(0, thumbIndent);
  }

  return layout;
}

void BlocksLookAndFeel::drawButtonBackground(Graphics& g, Button& button, const Colour& backgroundColour, bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) {
  if (button.getProperties()["hasBackground"].equals(true)) {
    auto cornerSize = button.getHeight() / 1.75f;
    auto bounds = button.getLocalBounds().toFloat().reduced(0.5f, 0.5f);

    auto baseColour = Colours::transparentBlack;

    if (shouldDrawButtonAsDown || shouldDrawButtonAsHighlighted)
      baseColour = baseColour.contrasting(shouldDrawButtonAsDown ? 0.2f : 0.05f);

    g.setColour(baseColour);

    auto flatOnLeft = button.isConnectedOnLeft();
    auto flatOnRight = button.isConnectedOnRight();
    auto flatOnTop = button.isConnectedOnTop();
    auto flatOnBottom = button.isConnectedOnBottom();

    if (flatOnLeft || flatOnRight || flatOnTop || flatOnBottom) {
      Path path;
      path.addRoundedRectangle(bounds.getX(), bounds.getY(),
        bounds.getWidth(), bounds.getHeight(),
        cornerSize, cornerSize,
        !(flatOnLeft || flatOnTop),
        !(flatOnRight || flatOnTop),
        !(flatOnLeft || flatOnBottom),
        !(flatOnRight || flatOnBottom));

      g.fillPath(path);

      g.setColour(button.findColour(ComboBox::outlineColourId));
      g.strokePath(path, PathStrokeType(4.0f));
    } else {
      g.fillRoundedRectangle(bounds, cornerSize);

      g.setColour(Colours::white);///button.findColour (ComboBox::outlineColourId));
      g.drawRoundedRectangle(bounds, cornerSize, 2.0f);
    }
  }
}

// just copy & pasted from LookAndFeel_V4 and did the rounded corners 
void BlocksLookAndFeel::fillTextEditorBackground(Graphics& g, int width, int height, TextEditor& textEditor) {
  auto theme = ThemeManager::shared()->getCurrent();
  if (dynamic_cast<AlertWindow*> (textEditor.getParentComponent()) != nullptr) {
    g.setColour(Colour(theme.one));
    g.fillRoundedRectangle(0, 0, width, height, 6.0f);

    g.setColour(Colour(theme.two));
    g.drawHorizontalLine(height - 1, 0.0f, static_cast<float> (width));
  } else {
    g.setColour(Colour(theme.two));
    g.fillRoundedRectangle(0, 0, width, height, 6.0f);
  }
}

void BlocksLookAndFeel::drawTextEditorOutline(Graphics& g, int width, int height, TextEditor& textEditor) { }

CaretComponent* BlocksLookAndFeel::createCaretComponent(Component* keyFocusOwner) {
  auto c = new CaretComponent(keyFocusOwner);
  c->setColour(CaretComponent::caretColourId, Colours::white.withAlpha(0.7f));
  return c;
}