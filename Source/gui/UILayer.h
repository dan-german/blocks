/*
  ==============================================================================

    UILayer.h
    Created: 27 Jun 2021 1:28:08am
    Author:  Dan German

  ==============================================================================
*/

#pragma once

#include "gui/ModulatorsButton.h"
#include "gui/controls/BlocksButton.h"
#include "gui/SideMenu.h"
#include "gui/controls/PresetButtonComponent.h"
#include "gui/KeyboardComponent.h"
#include "gui/ValueAnimator.h"
#include "gui/modulators_list_model.h"
#include "gui/ModulatorsSideMenu.h"
#include "gui/controls/ButtonGridPopup.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/controls/SVGButton.h"
#include "connection.h"
#include "ThemeListener.h"

class UILayer: public juce::Component, ComponentMovementWatcher, ThemeListener  {
public:
  SideMenu connections_;
  ModulatorsSideMenu modulators_;
  PresetButtonComponent preset_button_;
  KeyboardComponent keyboard;

  SVGButton settingsButton;
  SVGButton matrixButton;
  SVGButton saveButton;
  SVGButton newPresetButton;
  SVGButton theme_button_;

  std::unique_ptr<ModulatorsButton> modulatorsButton;
  LabelButton update_button_;
  juce::Label version_label_;

  ModulationsListBoxModel connections_list_box_model_;

  UILayer(juce::MidiKeyboardState& keyboard_state, BlocksSlider::Listener* listener);
  ~UILayer() override;

  void resized() override;
  void setConnections(std::vector<std::shared_ptr<model::Connection>> connections);
  void setModulators(std::vector<std::shared_ptr<model::Module>> modulators);
  void componentMovedOrResized(bool wasMoved, bool wasResized) override {};
  void componentPeerChanged() override {};
  void componentVisibilityChanged() override {};
private:
  int edgeMargin = 16;
  void addSVGButton(SVGButton& button, const char* rawData, size_t size);
  void resizePresetButton();
  void setupSideMenus();
  void setupKeyboard();
  void setupSettingsButton();
  void addModulatorsButton();
  void resizeModulatorsButton();
  void resizeSettingsButton();
  void resizeSaveAndNewButtons();
  void themeChanged(Theme theme) override;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UILayer)
};
