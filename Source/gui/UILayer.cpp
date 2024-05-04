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
#include "version_config.h"
#include "gui/ThemeManager.h"
#include "settings/UserSettings.h"

UILayer::UILayer(juce::MidiKeyboardState& keyboard_state, BlocksSlider::Listener* listener):
  ComponentMovementWatcher(this),
  keyboard(keyboard_state, MidiKeyboardComponent::Orientation::horizontalKeyboard),
  modulators_(listener)
{
  addModulatorsButton();
  addAndMakeVisible(preset_button_);

  if (JUCEApplication::isStandaloneApp()) setupSettingsButton();

  addSVGButton(newPresetButton, BinaryData::plus_svg, BinaryData::plus_svgSize);
  addSVGButton(saveButton, BinaryData::save_svg, BinaryData::save_svgSize);
  addSVGButton(theme_button_, BinaryData::theme_svg, BinaryData::theme_svgSize);
  addSVGButton(matrixButton, BinaryData::matrix_svg, BinaryData::matrix_svgSize);

  setupSideMenus();

  matrixButton.on_click_ = [this]() {
    UserSettings::shared()->set("connections_menu_visible", juce::String("true"));
    connections_.setVisible(true);
  };

  connections_.exit_button_.on_click_ = [this]() {
    UserSettings::shared()->set("connections_menu_visible", juce::String("false"));
    connections_.setVisible(false);
  };

  // auto initialized = UserSettings::shared()->getString("ProfileInitialized", falseString);
  // if (initialized == "true") return;o
  auto empty_string = juce::String("");
  if (UserSettings::shared()->getString("connections_menu_visible", empty_string) == "true" ) connections_.setVisible(true);
  if (UserSettings::shared()->getString("modulators_menu_visible", empty_string) == "true") modulators_.setVisible(true);

  setupKeyboard();
  connections_list_box_model_.slider_listener_ = listener;
  setInterceptsMouseClicks(false, true);
  setOpaque(false);
  update_button_.text.setText("update", dontSendNotification);
  addChildComponent(update_button_);
  update_button_.on_click_ = [this]() { juce::URL("https://www.soonth.com").launchInDefaultBrowser(); };
  addAndMakeVisible(version_label_);
  version_label_.setText(BLOCKS_VERSION, dontSendNotification);
  version_label_.setFont(juce::Font(13));
  version_label_.setAlpha(0.25);
  ThemeManager::shared()->addListener(this);
}

UILayer::~UILayer() {
  removeAllChildren();
  keyboard.setLookAndFeel(nullptr);
  ThemeManager::shared()->removeListener(this);
}

void UILayer::themeChanged(Theme theme) {
  version_label_.setColour(Label::textColourId, theme.two.brighter(0.4));
}

void UILayer::addSVGButton(SVGButton& button, const char* rawData, size_t size) {
  button.setSVG(rawData, size);
  addAndMakeVisible(button);
}

void UILayer::addModulatorsButton() {
  modulatorsButton.reset(new ModulatorsButton());
  addAndMakeVisible(*modulatorsButton);
  modulatorsButton->on_click_ = [this]() {
    UserSettings::shared()->set("modulators_menu_visible", juce::String("true"));
    modulators_.setVisible(true);
  };

  modulators_.exit_button_.on_click_ = [this]() {
    UserSettings::shared()->set("modulators_menu_visible", juce::String("false"));
    modulators_.setVisible(false);
  };
}

void UILayer::setupSettingsButton() {
  addSVGButton(settingsButton, BinaryData::settings_svg, BinaryData::settings_svgSize);

  settingsButton.on_click_ = []() {
    if (auto holder = juce::StandalonePluginHolder::getInstance()) {
      holder->showAudioSettingsDialog();
    }
  };
}

void UILayer::setupKeyboard() {
  keyboard.setKeyWidth(20);
  addAndMakeVisible(keyboard);
}

void UILayer::resized() {
  int keyboardHeight = 55;
  keyboard.setBounds(0, getHeight() - keyboardHeight, getWidth(), keyboardHeight);

  resizePresetButton();
  int sidePanelWidth = 260;
  connections_.setBounds(0, 0, sidePanelWidth - 20, getHeight() - keyboardHeight);
  modulators_.setBounds(getWidth() - sidePanelWidth, 0, sidePanelWidth, getHeight() - keyboardHeight);

  resizeSaveAndNewButtons();

  if (JUCEApplication::isStandaloneApp()) resizeSettingsButton();

  auto newSize = 22;
  auto newY = preset_button_.getY() + (preset_button_.getHeight() / 2) - newSize / 2;
  matrixButton.setBounds(edgeMargin, newY, 24, 24);

  resizeModulatorsButton();

  int themeButtonSize = 24;
  int y = keyboard.getY() - themeButtonSize - edgeMargin;
  theme_button_.setBounds(edgeMargin, y, themeButtonSize, themeButtonSize);
  version_label_.setBounds(theme_button_.getRight() + 5, y, 100, themeButtonSize);
}

void UILayer::resizeSaveAndNewButtons() {
  int buttonSize = 23;
  int verticalSpacing = 12;
  int saveButtonX = preset_button_.getRight() + verticalSpacing + buttonSize / 2;

  saveButton.setBounds(0, 0, buttonSize, buttonSize);
  saveButton.setCentrePosition(saveButtonX, preset_button_.getBounds().getCentreY());

  int newPresetX = preset_button_.getX() - verticalSpacing - buttonSize / 2;
  newPresetButton.setBounds(0, 0, buttonSize, buttonSize);
  newPresetButton.setCentrePosition(newPresetX, preset_button_.getBounds().getCentreY());

  int update_button_width = 50;
  update_button_.setBounds(0, 0, update_button_width, 23);
  update_button_.setCentrePosition(saveButton.getRight() + verticalSpacing + update_button_width / 2, preset_button_.getBounds().getCentreY());
}

void UILayer::resizeSettingsButton() {
  int settingsButtonSize = 25;
  int settingsY = keyboard.getY() - settingsButtonSize - edgeMargin;
  int settingsX = getWidth() - settingsButtonSize - edgeMargin;
  settingsButton.setBounds(settingsX, settingsY, settingsButtonSize, settingsButtonSize);
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
  connections_.listBox.setModel(&connections_list_box_model_);
  modulators_.isOnLeft = false;
}

void UILayer::setConnections(std::vector<std::shared_ptr<model::Connection>> modulationConnections) {
  connections_list_box_model_.setConnections(modulationConnections);
  if (connections_.listBox.isVisible()) {
    std::cout << "very very" << std::endl;
    connections_.listBox.updateContent();
  }
}

void UILayer::setModulators(std::vector<std::shared_ptr<model::Module>> newModulators) {
  modulators_.modulators_list_model_.setModulators(newModulators);
  if (modulators_.listBox.isVisible()) modulators_.listBox.updateContent();
}
