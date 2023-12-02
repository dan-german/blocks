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
#include "model/Modulation.h"
#include "gui/controls/PresetButtonComponent.h"
#include "gui/KeyboardComponent.h"
#include "gui/ValueAnimator.h"
#include "model/Module.h"
#include "gui/ModulatorsListModel.h"
#include "gui/ModulatorsSideMenu.h"
#include "gui/controls/ButtonGridPopup.h"
#include <juce_gui_basics/juce_gui_basics.h>
#include "gui/controls/SVGButton.h"
#include "gui/controls/SavePopup.h"
#include "connection.h"

using Modulation = Model::Modulation;
using Module = Model::Module;

class UILayer: public juce::Component, ComponentMovementWatcher {
public:
  SideMenu connections;
  ModulatorsSideMenu modulators_;
  PresetButtonComponent preset_button_;
  KeyboardComponent keyboard;

  std::unique_ptr<SVGButton> settingsButton;
  std::unique_ptr<SVGButton> matrixButton;
  std::unique_ptr<SVGButton> saveButton;
  std::unique_ptr<SVGButton> newPresetButton;
  std::unique_ptr<SVGButton> theme_button_;

  std::unique_ptr<ModulatorsButton> modulatorsButton;

  ModulationsListBoxModel connections_list_box_model_;

  UILayer(juce::MidiKeyboardState& keyboard_state, Slider::Listener* listener);
  ~UILayer() override;

  void resized() override;
  void setConnections(std::vector<std::shared_ptr<model::Connection>> connections);
  void setModulators(std::vector<std::shared_ptr<model::Module>> modulators);
  void componentMovedOrResized(bool wasMoved, bool wasResized) override {};
  void componentPeerChanged() override {};
  void componentVisibilityChanged() override {};
private:
  void addSVGButton(std::unique_ptr<SVGButton>& button, const char* rawData, size_t size);
  void resizePresetButton();
  void setupSideMenus();
  void setupKeyboard();
  void setupSettingsButton();
  void addModulatorsButton();
  void resizeModulatorsButton();
  void showModulatorsSideMenu();
  void resizeSettingsButton();
  void resizeSaveAndNewButtons();

private:
  int edgeMargin = 16;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(UILayer)
};
