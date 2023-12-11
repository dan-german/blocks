/*
  ==============================================================================

    UILayer.cpp
    Created: 27 Jun 2021 1:28:08am
    Author:  Dan German

  ==============================================================================
*/

#include "gui/UILayer.h"
#include <juce_audio_plugin_client/Standalone/juce_StandaloneFilterWindow.h>
#include "model/ModelConstants.h"
#include "BinaryData.h"
#include "module_new.h"

UILayer::UILayer(juce::MidiKeyboardState& keyboard_state, Slider::Listener* listener):
  keyboard(keyboard_state, MidiKeyboardComponent::Orientation::horizontalKeyboard), ComponentMovementWatcher(this) {
  addModulatorsButton();
  addAndMakeVisible(preset_button_);

  if (JUCEApplication::isStandaloneApp()) setupSettingsButton();

  addSVGButton(newPresetButton, BinaryData::plus_svg, BinaryData::plus_svgSize);
  addSVGButton(saveButton, BinaryData::save_svg, BinaryData::save_svgSize);
  addSVGButton(theme_button_, BinaryData::theme_svg, BinaryData::theme_svgSize);
  addSVGButton(matrixButton, BinaryData::matrix_svg, BinaryData::matrix_svgSize);

  setupSideMenus();

  matrixButton->on_click_ = [this]() { connections.setVisible(true); };

  setupKeyboard();
  connections_list_box_model_.slider_listener_ = listener;
  setInterceptsMouseClicks(false, true);
  setOpaque(false);
}

void UILayer::addSVGButton(std::unique_ptr<SVGButton>& button, const char* rawData, size_t size) {
  button.reset(new SVGButton());
  button->setSVG(rawData, size);
  addAndMakeVisible(*button);
}

void UILayer::addModulatorsButton() {
  modulatorsButton.reset(new ModulatorsButton());
  addAndMakeVisible(*modulatorsButton);

  modulatorsButton->on_click_ = [this]() { modulators_.setVisible(true); };
}

void UILayer::setupSettingsButton() {
  addSVGButton(settingsButton, BinaryData::settings_svg, BinaryData::settings_svgSize);

  settingsButton->on_click_ = []() {
    if (auto holder = juce::StandalonePluginHolder::getInstance()) {
      holder->showAudioSettingsDialog();
    }
  };
}

void UILayer::setupKeyboard() {
  keyboard.setKeyWidth(20);
  addAndMakeVisible(keyboard);
}

UILayer::~UILayer() {
  removeAllChildren();
  keyboard.setLookAndFeel(nullptr);
}

void UILayer::resized() {
  int keyboardHeight = 55;
  keyboard.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);

  resizePresetButton();
  int sidePanelWidth = 260;
  connections.setBounds(0, 0, sidePanelWidth, getHeight() - keyboardHeight);
  modulators_.setBounds(getWidth() - sidePanelWidth, 0, sidePanelWidth, getHeight() - keyboardHeight);

  resizeSaveAndNewButtons();

  if (JUCEApplication::isStandaloneApp()) resizeSettingsButton();

  auto newSize = 22;
  auto newY = preset_button_.getY() + (preset_button_.getHeight() / 2) - newSize / 2;
  matrixButton->setBounds(edgeMargin, newY, 24, 24);

  resizeModulatorsButton();

  int themeButtonSize = 24;
  int y = keyboard.getY() - themeButtonSize - edgeMargin;
  theme_button_->setBounds(edgeMargin, y, themeButtonSize, themeButtonSize);
}

void UILayer::resizeSaveAndNewButtons() {
  int buttonSize = 23;
  int verticalSpacing = 12;
  int saveButtonX = preset_button_.getRight() + verticalSpacing + buttonSize / 2;

  saveButton->setBounds(0, 0, buttonSize, buttonSize);
  saveButton->setCentrePosition(saveButtonX, preset_button_.getBounds().getCentreY());

  int newPresetX = preset_button_.getX() - verticalSpacing - buttonSize / 2;
  newPresetButton->setBounds(0, 0, buttonSize, buttonSize);
  newPresetButton->setCentrePosition(newPresetX, preset_button_.getBounds().getCentreY());
}

void UILayer::resizeSettingsButton() {
  int settingsButtonSize = 25;
  int settingsY = keyboard.getY() - settingsButtonSize - edgeMargin;
  int settingsX = getWidth() - settingsButtonSize - edgeMargin;
  settingsButton->setBounds(settingsX, settingsY, settingsButtonSize, settingsButtonSize);
}

void UILayer::resizeModulatorsButton() {
  int modulatorsButtonWidth = 32;
  modulatorsButton->setBounds(getWidth() - modulatorsButtonWidth - edgeMargin, preset_button_.getY(), modulatorsButtonWidth, 15);
}

void UILayer::resizePresetButton() {
  int width = 170;
  int verticalSpacing = 15;
  preset_button_.setBounds(getWidth() / 2 - width / 2, verticalSpacing, width, Constants::tabHeight);
}

void UILayer::setupSideMenus() {
  addChildComponent(connections);
  connections.listBox.setModel(&connections_list_box_model_);
  modulators_.isOnLeft = false;
}

void UILayer::showModulatorsSideMenu() {
  modulators_.listBox.updateContent();
  modulators_.setVisible(false);
}

void UILayer::setConnections(std::vector<std::shared_ptr<model::Connection>> modulationConnections) {
  connections_list_box_model_.setConnections(modulationConnections);
  if (connections.listBox.isVisible()) connections.listBox.updateContent();
}

void UILayer::setModulators(std::vector<std::shared_ptr<model::Module>> newModulators) {
  modulators_.modulators_list_model_.setModulators(newModulators);
  if (modulators_.listBox.isVisible()) modulators_.listBox.updateContent();
}
