/*
  ==============================================================================

    ConnectionComponent.h
    Created: 11 Jun 2021 10:39:21pm
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/controls/BlocksButton.h"
#include "gui/ModulationIndicatorComponent.h"
#include "gui/controls/ExitButton.h"

#include "gui/ThemeManager.h"
class ConnectionComponent: public juce::Component, public ThemeListener {
public:
  struct Listener;
  Listener* delegate;
  ConnectionComponent();
  ~ConnectionComponent() override;

  void paint(juce::Graphics&) override;
  void resized() override;
  void handleOscGainEnvelope();
  void reset();

  Slider slider;
  Label source;
  Label target;
  BlocksButton bipolarButton;
  ModulationIndicatorComponent indicator;
  ExitButton exitButton;
  int row = -1;

  void themeChanged(Theme theme) override;
private:
  int horizontalInsets = 3;
  void resizeBipolarButton();
  void setupBipolarButton();
  void resizeExitButton();
  void resizeIndicator();
  void resizeSlider();

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ConnectionComponent)
};

struct ConnectionComponent::Listener {
  virtual void modulationConnectionBipolarPressed(ConnectionComponent* component, bool bipolar) = 0;
  virtual void connectionDeleted(ConnectionComponent* component) = 0;
  virtual ~Listener() = default;
};
