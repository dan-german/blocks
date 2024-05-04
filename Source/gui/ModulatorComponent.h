/*
  ==============================================================================

    ModulatorComponent.h
    Created: 29 Oct 2021 2:44:00am
    Author:  Dan German

  ==============================================================================
*/

#pragma once
#include "DragIndicatorComponent.h"
#include "DotComponent.h"
#include "gui/OscillatorPainter.h"
#include "gui/EnvelopePath.h"
#include "gui/controls/ExitButton.h"
#include "gui/ThemeManager.h"
#include "gui/slider_container.h"

class ModulatorComponent: public Component, Slider::Listener, ThemeListener {
private:
  ExitButton exitButton;
  DragIndicatorComponent drag_indicator_;
  Component drag_hitbox_;
  Component slidersContainer;
  Rectangle<int> bounds;
  const int sliderHeight = 23;
  const int dragIndicatorHeight = 21;
  const int dragIndicatorBottomSpacing = 6;
  const int sliderSpacing = 2;
  const int columns = 2;
  const int topSpacing = 16;
  const int sliderHorizontalSpacing = 3;
  const int offset = 0;
  int sliderHorizontalInsets = 6;
  Colour colour;
  BlocksSlider::Listener* blocks_slider_listener;

  void sliderValueChanged(Slider* slider) override;
  void sliderDragStarted(Slider* slider) override;
  void sliderDragEnded(Slider* slider) override;

  int currentSliderIndex = -1;
public:
  gui::SliderContainer slider_container_;
  std::map<Slider*, std::string> slider_parameter_name_map_;
  struct Listener;
  Listener* delegate_;
  OwnedArray<LabeledSlider> sliders;
  Label title;
  EnvelopePath envelopePath;
  OscillatorPainter oscillatorPainter;
  // std::function<void(int, float)> onSliderValueChange;
  int row = -1;
  ID model_id_;

  ModulatorComponent(BlocksSlider::Listener* listener);
  ~ModulatorComponent() override;

  int calculateHeight();
  void paint(Graphics& g) override;
  void resized() override;
  void resizeSliders();
  void resizeTitle();
  void setupTitle();
  void setupSliders();
  void resizeSliderContainer();
  void resizeDragIndicator();
  void setColour(Colour colour);
  Colour getColour() { return colour; }
  void mouseDown(const MouseEvent& event) override;
  void mouseDrag(const MouseEvent& event) override;
  void mouseUp(const MouseEvent& event) override;
  void mouseEnter(const MouseEvent& event) override;
  void mouseExit(const MouseEvent& event) override;
  inline static String dragComponentIdentifier = "dragComponentIdentifier";
  void resizeOscillatorPainter();
  void resizeExitButton();
  void setupRemoveButton();
  void drawBottomLine(Graphics& g) const;
  void resizeEnvelopePath();
  void themeChanged(Theme theme) override;
};

struct ModulatorComponent::Listener {
  virtual ~Listener() = default;
  virtual void modulatorEndedDrag(ModulatorComponent* modulatorComponent, const MouseEvent& event) = 0;
  virtual void modulatorIsDragging(ModulatorComponent* modulatorComponent, const MouseEvent& event) = 0;
  virtual void modulatorStartedDrag(ModulatorComponent* modulatorComponent, const MouseEvent& event) = 0;
  virtual void modulatorRemoved(ModulatorComponent* modulatorComponent) = 0;
  virtual void modulatorIsAdjusting(ModulatorComponent* modulatorComponent, std::string parameter_name, float value) = 0;
  virtual void modulatorGestureChanged(ModulatorComponent* modulatorComponent, std::string paramter_name, bool started) = 0;
};
