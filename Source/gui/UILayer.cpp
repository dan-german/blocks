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

UILayer::UILayer(juce::MidiKeyboardState& keyboard_state, Slider::Listener* listener): 
    keyboard(keyboard_state, MidiKeyboardComponent::Orientation::horizontalKeyboard), ComponentMovementWatcher(this) {
  addModulatorsButton();
  addAndMakeVisible(presetButton);

  if (JUCEApplication::isStandaloneApp()) setupSettingsButton();

  addSVGButton(newPresetButton, BinaryData::plus_svg, BinaryData::plus_svgSize);
  addSVGButton(saveButton, BinaryData::save_svg, BinaryData::save_svgSize);
  addSVGButton(themeButton, BinaryData::theme_svg, BinaryData::theme_svgSize);
  addSVGButton(matrixButton, BinaryData::matrix_svg, BinaryData::matrix_svgSize);

  setupSideMenus();

  matrixButton->onClick = [this]() { matrix.setVisible(true); };

  setupKeyboard();
  modulationsListBoxModel.sliderListener = listener;
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

  modulatorsButton->onClick = [this]() { modulators.setVisible(true); };
}

void UILayer::setupSettingsButton() {
  addSVGButton(settingsButton, BinaryData::settings_svg, BinaryData::settings_svgSize);

  settingsButton->onClick = []() {
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
  matrix.setBounds(0, 0, sidePanelWidth, getHeight() - keyboardHeight);
  modulators.setBounds(getWidth() - sidePanelWidth, 0, sidePanelWidth, getHeight() - keyboardHeight);

  resizeSaveAndNewButtons();

  if (JUCEApplication::isStandaloneApp()) resizeSettingsButton();

  auto newSize = 22;
  auto newY = presetButton.getY() + (presetButton.getHeight() / 2) - newSize / 2;
  matrixButton->setBounds(edgeMargin, newY, 24, 24);

  resizeModulatorsButton();

  int themeButtonSize = 24;
  int y = keyboard.getY() - themeButtonSize - edgeMargin;
  themeButton->setBounds(edgeMargin, y, themeButtonSize, themeButtonSize);
}

void UILayer::resizeSaveAndNewButtons() {
  int buttonSize = 23;
  int verticalSpacing = 12;
  int saveButtonX = presetButton.getRight() + verticalSpacing + buttonSize / 2;

  saveButton->setBounds(0, 0, buttonSize, buttonSize);
  saveButton->setCentrePosition(saveButtonX, presetButton.getBounds().getCentreY());

  int newPresetX = presetButton.getX() - verticalSpacing - buttonSize / 2;
  newPresetButton->setBounds(0, 0, buttonSize, buttonSize);
  newPresetButton->setCentrePosition(newPresetX, presetButton.getBounds().getCentreY());
}

void UILayer::resizeSettingsButton() {
  int settingsButtonSize = 25;
  int settingsY = keyboard.getY() - settingsButtonSize - edgeMargin;
  int settingsX = getWidth() - settingsButtonSize - edgeMargin;
  settingsButton->setBounds(settingsX, settingsY, settingsButtonSize, settingsButtonSize);
}

void UILayer::resizeModulatorsButton() {
  int modulatorsButtonWidth = 32;
  modulatorsButton->setBounds(getWidth() - modulatorsButtonWidth - edgeMargin, presetButton.getY(), modulatorsButtonWidth, 15);
}

void UILayer::resizePresetButton() {
  int width = 170;
  int verticalSpacing = 15;
  presetButton.setBounds(getWidth() / 2 - width / 2, verticalSpacing, width, Constants::tabHeight);
}

void UILayer::setupSideMenus() {
  addChildComponent(matrix);
  matrix.listBox.setModel(&modulationsListBoxModel);
  modulators.isOnLeft = false;
}

void UILayer::showModulatorsSideMenu() {
  modulators.listBox.updateContent();
  modulators.setVisible(false);
}

void UILayer::setModulations(Array<std::shared_ptr<Modulation>> modulationConnections) {
  modulationsListBoxModel.setConnections(modulationConnections);
  if (matrix.listBox.isVisible()) matrix.listBox.updateContent();
}

void UILayer::setModulators(Array<std::shared_ptr<Module>> newModulators) {
  modulators.modulatorsListModel.setModulators(newModulators);
  if (modulators.listBox.isVisible()) modulators.listBox.updateContent();
}
