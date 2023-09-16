/*
  ==============================================================================

    InspectorSlider.h
    Created: 20 Sep 2020 6:41:38pm
    Author:  dange

  ==============================================================================
*/

#pragma once

#include "HighlightComponent.h"
#include "ModulationIndicatorComponent.h"

class InspectorSlider: public Component, public Slider::Listener {
public:
  struct Delegate {
    virtual void modulatorMagnitudeChanged(InspectorSlider* parameterSlider, int modulatorIndex, float value) = 0;
    virtual ~Delegate() = default;
  };

  enum class Type { regular, normal, thumb };

  Type type;
  Delegate* delegate;
  Slider slider;
  Label titleLabel;

  InspectorSlider(Slider::SliderStyle style = Slider::LinearVertical, Slider::TextEntryBoxPosition textBoxPosition = Slider::TextBoxBelow);
  ~InspectorSlider() override;

  void setType(Type type);
  void setHighlighted(bool highlighted, Colour colour = Colours::white.withAlpha(0.25f));
  void sliderValueChanged(Slider* slider) override;
  void addModulationIndicator(float magnitude, Colour colour, bool bipolar, float value);
  void setModulatorValue(int modulatorIndex, float magnitude, float value);
  void setModulatorBipolar(int modulatorIndex, bool bipolar);
  void removeIndicator(int index);
  void setValue(float value);
  HighlightComponent highlightComponent;
protected:
  void resized() override;
private:
  int sliderVerticalInsets = 0;
  void resizeIndicators();
  OwnedArray<ModulationIndicatorComponent> indicators;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(InspectorSlider)
};
