#include "MainComponent.h"
#include "settings/GridConfigs.h"
#include "gui/ThemeManager.h"
#include "settings/UserSettings.h"
#include "module_new.h"
#include "vital/common/synth_constants.h"
#include "version_config.h"

MainComponent::MainComponent(juce::MidiKeyboardState& keyboard_state, Delegate* delegate):
  delegate(delegate),
  ui_layer_(keyboard_state, this),
  tab_grid_(GridConfigs::tab),
  block_grid_(GridConfigs::blocks)
{
  setWantsKeyboardFocus(false);

  setLookAndFeel(&blocks_laf_);
  setupPopupMenus();
  setupInspector();
  setupUI();
  setupListeners();
  addAndMakeVisible(block_placeholder_);
  addChildComponent(cursor, 1000);
  cursor.setInterceptsMouseClicks(false, false);
  setupPresetMenu();
  setupDarkBackground(&grid_dark_background_, 3);
  addChildComponent(ui_layer_.modulators_);
  addChildComponent(ui_layer_.connections);
  setupTabGrid();
  setupBlockGrid();
  setupDarkBackground(&dark_background_, -1);
  addAndMakeVisible(column_controls_);
  addAndMakeVisible(selection_rect_);
  column_controls_.listener = this;
  note_logger_.listener = this;
  ThemeManager::shared()->set(UserSettings::shared()->getInt("theme", 0));

  auto req = [this] {
    auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress);
    auto version = juce::URL("https://blocksbucket.s3.us-east-2.amazonaws.com/version").createInputStream(options)->readEntireStreamAsString().toStdString();
    juce::MessageManager::callAsync([this, version] { ui_layer_.update_button_.setVisible(version != BLOCKS_VERSION);});
  };

  juce::Thread::launch(req);
  // slider.setInterceptsMouseClicks(false, false);
  addAndMakeVisible(slider);
  slider.setName("slider");
  slider.addMouseListener(this, true);
}

void MainComponent::updateDotPosition(const Point<int> position) {
  cursor.setBounds(position.getX(), position.getY(), cursor.size, cursor.size);
  repaint();
}

// void MainComponent::modulatorStartedAdjusting(ModulatorComponent* modulatorComponent, int index) {
//   is_modulator_adjusting_ = true;
  // delegate->editorParameterGestureChanged(delegate->getModulator(modulatorComponent->row)->name, index, true);
// }

// void MainComponent::modulatorEndedAdjusting(ModulatorComponent* modulatorComponent, int index) {
//   is_modulator_adjusting_ = false;
  // delegate->editorParameterGestureChanged(delegate->getModulator(modulatorComponent->row)->name, index, false);
// }

void MainComponent::modulatorIsAdjusting(ModulatorComponent* component, std::string parameter_name, float value) {
  delegate->editorAdjustedModulator(parameter_name, component->row, value);
}

void MainComponent::paint(juce::Graphics& g) {
  g.fillAll(ThemeManager::shared()->getCurrent().background);
}

void MainComponent::inspectorGestureChanged(std::string parameter_name, bool started) {
  is_adjusting_inspector_ = started;
  std::shared_ptr<model::Module> focusedModule = delegate->getBlock2(focused_grid_item_->index);

  // auto isTab = tabGrid.containsItem(focusedGridItem);
  // if (isTab) {
  //   focusedModule = delegate->getTab(focusedGridItem->index.column);
  // } else {
  //   focusedModule = delegate->getBlock(focusedGridItem->index);
  // }

  delegate->editorParameterGestureChanged(focusedModule->name, parameter_name, started);
}

void MainComponent::changeModulePainter(int value) {
  auto cast_block = static_cast<BlockComponent*>(focused_grid_item_);
  cast_block->getPainter()->setWaveformType(BlockComponent::getWaveformType(value));
}

MainComponent::~MainComponent() {
  setLookAndFeel(nullptr);
}

void MainComponent::setupDarkBackground(DarkBackground* darkBackgroundComponent, int layer) {
  addChildComponent(darkBackgroundComponent, layer);
  darkBackgroundComponent->setOpaque(false);
  darkBackgroundComponent->setInterceptsMouseClicks(true, true);
  darkBackgroundComponent->onClick = [this](Component* c) { darkBackgroundClicked(c); };
}

std::vector<Component*> MainComponent::allPopups() {
  return { &blocks_popup_, &modulators_popup_, &presets_popup_, &save_popup_, &copy_delete_popup_, &paste_popup_ };
}

void MainComponent::darkBackgroundClicked(Component* component) {
  component->setVisible(false);

  for (auto popup : allPopups()) {
    if (popup->isVisible()) {
      popup->setVisible(false);
    }
  }

  if (focused_grid_item_) {
    auto currentGrid = static_cast<GridComponent*>(focused_grid_item_->getParentComponent()); // ehh
    toggleGridItemSelection(currentGrid, focused_grid_item_, false);
  }

  tab_grid_.hideAllItems(false);
  block_grid_.hideAllItems(false);
}

void MainComponent::setupTabGrid() {
  tab_grid_.listener = this;
  // addAndMakeVisible(tab_grid_, 2);

  for (auto glow_indicator_ : tab_grid_.glowIndicators) {
    addChildComponent(glow_indicator_);
    glow_indicator_->toFront(false);
  }
}

void MainComponent::setupPresetMenu() {
  addChildComponent(presets_popup_);
  presets_popup_.scrollMode = true;
  presets_popup_.itemHeight = 23;
}

void MainComponent::setupBlockGrid() {
  addAndMakeVisible(block_grid_, 4);
  block_grid_.addMouseListener(this, true);
  block_grid_.listener = this;
}

void MainComponent::setupUI() {
  // uiLayer.presetButton.setStrings(delegate->editorRequestsPresetNames());
  ui_layer_.connections_list_box_model_.delegate_ = this;
  ui_layer_.modulators_.addMouseListener(this, true);
  // ui_layer_.modulators_.addMouseListener(this, true);
  addAndMakeVisible(ui_layer_, 2);
}

void MainComponent::modulatorStartedDrag(ModulatorComponent* component, const MouseEvent& event) {
  updateDotPosition(event.getEventRelativeTo(this).getPosition());
  enterModulatorDragMode(component->getColour());
}

void MainComponent::setupListeners() {
  timer_.start();
  timer_.callback = [this](float secondsSinceUpdate) { this->graphicsTimerCallback(secondsSinceUpdate); };

  ui_layer_.modulators_.modulators_list_model_.modulator_listener = this;

  save_popup_.saveButton.on_click_ = [this]() {
    auto name = save_popup_.textEditor.getText();
    bool hasTabs = tab_grid_.getItems().size() > 0;
    bool hasModulators = ui_layer_.modulators_.modulators_list_model_.getNumRows() > 0;
    bool hasBlocks = block_grid_.getItems().size() > 0;

    if (name.isEmpty()) return;
    if (!hasTabs && !hasModulators && !hasBlocks) return;

    delegate->editorSavedPreset(name.toStdString());
    ui_layer_.preset_button_.content.label.setText(name, dontSendNotification);
    ui_layer_.preset_button_.setStrings(delegate->editorRequestsPresetNames());
    save_popup_.setVisible(false);
  };

  ui_layer_.modulators_.plus_button_callback = [this](const juce::MouseEvent& event) {
    auto position = event.eventComponent->getPosition() + ui_layer_.modulators_.getPosition();
    modulators_popup_.setBounds(position.getX(), position.getY(), 74, 76);
    showPopup(modulators_popup_, [this](Index i) { this->clickOnModulatorsPopup(i); });
  };

  ui_layer_.newPresetButton->on_click_ = [this]() {
    delegate->editorChangedPreset(-1);
    clear();
  };

  ui_layer_.preset_button_.on_arrow_click_ = [this](bool right) {
    if (auto preset = delegate->editorNavigatedPreset(right)) {
      clear();
      loadState(*preset);
    }
  };

  ui_layer_.saveButton->on_click_ = [this]() {
    this->dark_background_.setVisible(true);
    this->dark_background_.toFront(true);
    this->save_popup_.setBounds(0, 0, 210, 37);

    int y = this->getHeight() / 12;
    this->save_popup_.setCentrePosition(ui_layer_.saveButton->getBounds().getCentre().x, y);
    this->save_popup_.present();
  };

  ui_layer_.modulators_.modulators_list_model_.modulator_listener = this;

  ui_layer_.modulators_.on_added_modulator_ = [this](int index) {
    addModulator(model::modulators[index].toStdString());
  };
  ui_layer_.preset_button_.on_click_ = [this]() { presetButtonClicked(); };
  ui_layer_.theme_button_->on_click_ = [this]() {
    UserSettings::shared()->set("theme", ThemeManager::shared()->next());
    repaint();
  };
}

void MainComponent::addModulator(std::string code) {
  auto module = delegate->editorAddedModulator2(code);
  if (module == nullptr) return;
  ui_layer_.setModulators(delegate->getModulators2());
}

void MainComponent::resized() {
  ui_layer_.setBounds(getLocalBounds());
  resizeGrid();
  resizeColumnControls();
  ResizeInspector();
  resizeTabContainer();
  dark_background_.path.addRectangle(getLocalBounds());
  dark_background_.setBounds(getLocalBounds());
  grid_dark_background_.path.addRectangle(getLocalBounds());
  grid_dark_background_.setBounds(getLocalBounds());

  for (auto glowIndicator : tab_grid_.glowIndicators) {
    auto bounds = tab_grid_.boundsForItem(glowIndicator, true);
    bounds.removeFromTop(20);
    glowIndicator->setBounds(bounds);
  }
  slider.setBounds(block_grid_.getBounds().withY(block_grid_.getY() + block_grid_.getHeight() + 100).withHeight(20).withWidth(40));
}

void MainComponent::resizeColumnControls() {
  auto bounds = block_grid_.getBounds().withHeight(42).withY(block_grid_.getY() + block_grid_.getHeight() + 19);
  column_controls_.setBounds(bounds);
}

void MainComponent::resizeTabContainer() {
  int height = Constants::tabHeight;
  int verticalSpacing = 19;
  tab_grid_.setBounds(block_grid_.getX(), block_grid_.getY() - height - verticalSpacing, block_grid_.getWidth(), height);
}

void MainComponent::mouseDown(const MouseEvent& event) {
  auto relative_event = event.getEventRelativeTo(this);
  // auto detectedComponent = getComponentAt(relative_event.getPosition());
  // printf("x: %d, y: %d\n", relative_event.getPosition().getX(), relative_event.getPosition().getY());
  // if (detectedComponent != nullptr) {
  //   printf("Component at point: %s\n", detectedComponent->getName().toStdString().c_str());
  // } else {
  //   printf("No component at point\n");
  // }
  if (auto* module = dynamic_cast<GridComponent*>(event.eventComponent))
    return;
}

void MainComponent::mouseUp(const MouseEvent& event) {
  if (modulator_drag_mode_) {
    previous_slider_under_mouse_ = {};
    modulator_drag_mode_ = false;
    return;
  }

  selection_rect_.setBounds(Rectangle<int>());
  auto grid_relative_position = event.getEventRelativeTo(&block_grid_).getPosition();
  bool is_mouse_on_block_grid = block_grid_.contains(grid_relative_position);
  bool was_dragging = event.getEventRelativeTo(this).getDistanceFromDragStart() > 2;
  if (was_dragging) {
    if (currently_selected_items_.size() == 1) {
      toggleGridItemSelection(&block_grid_, currently_selected_items_[0], true);
    } else if (is_mouse_on_block_grid) {
      block_grid_.add_button_.setAlpha(1);
    }
    return;
  }

  if (focused_grid_item_) {
    toggleGridItemSelection(&block_grid_, focused_grid_item_, false);
  }

  currently_selected_items_ = {};
  if (multiple_selection_) {
    block_grid_.setAllItemsSelected(false);
  }

  setMouseCursor(MouseCursor::NormalCursor);
  for (auto popup : allPopups()) { popup->setVisible(false); }

  auto componentName = event.eventComponent->getName();

  if (componentName == "ModulatorsPlusButton") {
    // auto position = event.eventComponent->getPosition() + ui_layer_.modulators_.getPosition();
    // modulators_popup_.setBounds(position.getX(), position.getY(), 74, 74);
    // showPopup(modulators_popup_, [this](Index i) { this->clickOnModulatorsPopup(i); });
  } else if (componentName == "PresetMainButton") {
    auto componentY = event.eventComponent->getPosition().getY();
    auto point = event.eventComponent->getPosition().withY(componentY + 8);
    showPopup(presets_popup_, [this](Index i) { this->clickOnModulatorsPopup(i); });
  }

  if (is_mouse_on_block_grid) {
    if (event.mods.isLeftButtonDown()) {
      // can this be moved to the grid component?
      auto mouse_down_position = event.getEventRelativeTo(&block_grid_).getMouseDownPosition();
      Index index = block_grid_.indexForPoint(grid_relative_position);
      bool slot_taken = block_grid_.isSlotTaken(index);

      Index starting_index = block_grid_.indexForPoint(mouse_down_position);
      if (starting_index == index && !slot_taken) {
        clickOnGrid(index);
      }
    } else {
      handlePastePopup(event);
    }
  }

  cursor.setVisible(false);
}

void MainComponent::handlePastePopup(const juce::MouseEvent& event) {
  if (copied_blocks_.size() == 0) return;
  Array<StringArray> model { { "paste" } };
  paste_popup_.setModel(model);
  auto relative_position = event.getEventRelativeTo(this);
  paste_popup_.setBounds(relative_position.getPosition().getX(), relative_position.getPosition().getY(), 56, 40);
  showPopup(paste_popup_, [this, event](Index i) {
    if (this->copied_blocks_.size() > 0) {
      auto grid_relative_position = event.getEventRelativeTo(&block_grid_);
      Index target = block_grid_.indexForPoint(grid_relative_position.getPosition());
      auto new_blocks = this->delegate->editorPastedIndices(this->copied_blocks_, target);
      for (auto block : new_blocks) {
        spawnBlockComponent(block);
      }
      ui_layer_.setConnections(delegate->getModulations());
    }
  });
}

void MainComponent::clickOnModulatorsPopup(Index index) {
  addModulator(model::modulators[index.row].toStdString());
  dark_background_.setVisible(false);
}

void MainComponent::clickOnGrid(Index& index) {
  if (blocks_popup_.isVisible()) {
    blocks_popup_.setVisible(false);
  } else {
    showBlocksPopup(index);
  }
}

void MainComponent::resizeGrid() {
  using namespace Constants;
  const float gridWidth = columns * blockWidth + (columns - 1) * moduleSpacing + gridEdgeSpacing * 2;
  const float gridHeight = rows * blockHeight + (rows - 1) * (moduleSpacing)+gridEdgeSpacing * 2;
  const float x = (getLocalBounds().getWidth() - gridWidth) / 2;

  int gridTopOffset = 135;
  block_grid_.setBounds(x, getLocalBounds().getY() + gridTopOffset, gridWidth, gridHeight);

  for (auto moduleComponent : block_grid_.getItems())
    block_grid_.resnapBlock(moduleComponent);
}

void MainComponent::showBlocksPopup(Index index) {
  block_grid_.add_button_.setAlpha(0);
  block_grid_.reset();

  auto blockSelectionCompletion = [this, index](Index selectedIndex) {
    int code = selectedIndex.column == 0 ? selectedIndex.row : selectedIndex.row + 5;
    auto module = addBlock(code, index);
    if (module == nullptr) return;
    spawnBlockComponent(module);
  };

  blocks_popup_.setBounds(
    block_grid_.add_button_.getX() + block_grid_.getX(),
    block_grid_.add_button_.getY() + block_grid_.getY(),
    140,
    120
  );

  showPopup(blocks_popup_, blockSelectionCompletion);
}

void MainComponent::showPopup(ButtonGridPopup& popup, std::function<void(Index)> callback) {
  dark_background_.setVisible(true);
  dark_background_.toFront(true);

  auto callbackWrapper = [this, callback, &popup](Index index) {
    dark_background_.setVisible(false);
    popup.setVisible(false);
    popup.setInterceptsMouseClicks(false, false);
    callback(index);
  };

  popup.present(callbackWrapper);
}

std::shared_ptr<model::Block> MainComponent::addBlock(int code, Index index) {
  std::shared_ptr<model::Block> block = nullptr;
  StringArray all;

  // add the model::block_popup_column_one string array to all
  for (auto s : model::block_popup_column_one) all.add(s);
  for (auto s : model::block_popup_column_two) all.add(s);


  // all.pu(model::block_popup_column_one);
  // all.addArray(model::block_popup_column_two);
  return delegate->editorAddedBlock2(all[code].toStdString(), index);
}

void MainComponent::setupInspector() {
  // inspector_.addMouseListener(this, true);
  inspector_.delegate = this;
  addChildComponent(inspector_);
}

void MainComponent::ResizeInspector() {
  auto gridY = block_grid_.getY() + block_grid_.getHeight();
  auto gridCenterX = block_grid_.getX() + block_grid_.getWidth() / 2;
  int inspectorX = gridCenterX - inspector_.calculateWidth() / 2;
  inspector_.setBounds(inspectorX, gridY + 60, 1, 220);
}

void MainComponent::dismissPopup(ButtonGridPopup& popup) {
  popup.setVisible(false);
}

void MainComponent::removeTab(GridItemComponent* tab) {
  if (tab == focused_grid_item_) {
    inspector_.setVisible(false);
    focused_grid_item_ = nullptr;
  } else if (inspector_.isVisible()) {
    refreshInspector();
  }

  removeChildComponent(tab);

  auto tab_column = tab->index.column;
  tab_grid_.detachModule(tab->index, true);

  delegate->editorRemovedTab(tab_column);
  grid_dark_background_.setVisible(false);
}

void MainComponent::removeBlock(GridItemComponent* block) {
  auto item = static_cast<BlockComponent*>(block);
  if (item == focused_grid_item_) {
    inspector_.setVisible(false);
    focused_grid_item_ = nullptr;
  } else if (inspector_.isVisible()) {
    refreshInspector();
  }
  blocks.removeFirstMatchingValue(item);

  auto index = block->index;
  block_grid_.detachModule(index);
  block_grid_.resetDotsVisibility();

  delegate->editorRemovedBlock(index);
  ui_layer_.setConnections(delegate->getModulations());
  block_grid_.setAllItemsSelected(false);
  currently_selected_items_ = {};
  // resetDownFlowingDots();
}

void MainComponent::handleModuleLandedOnInspector(BlockComponent* moduleComponent, const Point<int>& inspectorRelativePosition) {
  block_grid_.resnapBlock(moduleComponent);
}

void MainComponent::inspectorChangedParameter(int sliderIndex, float value) {
  Index moduleIndex(focused_grid_item_->index);
  auto isTab = tab_grid_.containsItem(focused_grid_item_);

  if (isTab) {
    delegate->editorAdjustedTab(moduleIndex.column, sliderIndex, value);
  } else {
    auto module = delegate->getBlock2(moduleIndex);
    delegate->editorAdjustedBlock(moduleIndex, sliderIndex, value);
    updateModuleComponentVisuals(sliderIndex, value, module);
  }
}

void MainComponent::updateModuleComponentVisuals(int sliderIndex, float value, std::shared_ptr<model::Module> module) {
  if (module->id.type == "osc") {
    if (module->parameters_[sliderIndex]->name == "wave") {
      changeModulePainter((int)value);
    }
    return;

    switch (sliderIndex) {
    case 0:
      changeModulePainter((int)value);
      break;
      // case OscillatorModule::pUnison: {
      //   // auto module_component = block_matrix_[block->index.row][block->index.column];
      //   // if (auto painter = module_component->getPainter())
      //   //   painter->setUnison(static_cast<int>(value));
      //   break;
      // }
    default:
      break;
    }
  }
}

void MainComponent::refreshInspector() {
  std::shared_ptr<model::Module> focused_module;

  // auto isTab = tabGrid.containsItem(focusedGridItem);
  // if (isTab) {
  //   focusedModule = delegate->getTab(focusedGridItem->index.column);
  // } else {
  focused_module = delegate->getBlock2(focused_grid_item_->index);
  // }

  inspector_.setConfiguration(focused_module);
  ResizeInspector();
}

// PopupMenu MainComponent::spawnModulationMenu(Module& target) {
//   PopupMenu modulateMenu;
//   modulateMenu.setLookAndFeel(&blocks_laf_);
//   for (int i = 0; i < target.parameters.size(); i++)
//     modulateMenu.addItem(i + 1, target.parameters[i]->id);
//   return modulateMenu;
// }

void MainComponent::spawnBlockComponent(std::shared_ptr<model::Block> block) {
  auto blockComponent = BlockComponent::create(block);

  blocks.add(blockComponent);
  block_grid_.addItem(blockComponent, block->index, true);
  block_matrix_[block->index.column][block->index.row] = blockComponent;
  addAndMakeVisible(blockComponent, 1000);
  cursor.setAlwaysOnTop(true);
  if (block->length > 1) block_grid_.setItemLength(blockComponent, block->length);
  blockComponent->setConfig(block, delegate->getModulations());
  // ResetDownFlowingDots();
}

// void MainComponent::spawnTabComponent(std::shared_ptr<Tab> tab) {
//   auto tabComponent = TabComponent::create(*tab, &tab_grid_);
//   addAndMakeVisible(tabComponent, 5);
//   tabComponent->toFront(false);
//   tab_grid_.addItem(tabComponent, { 0, tab->column }, true);
//   if (tab->length > 1) tab_grid_.setItemLength(tabComponent, tab->length);
//   for (auto glowIndicator : tab_grid_.glowIndicators)
//     glowIndicator->toFront(false);
// }

void MainComponent::highlightModulatableSliders(bool highlight, Colour color = Colours::red) {
  auto modulators = delegate->getModulators2();
  for (int i = 0; i < modulators.size(); i++) {
    if (auto mc = dynamic_cast<ModulatorComponent*>(ui_layer_.modulators_.listBox.getComponentForRowNumber(i))) {
      mc->highlightSliders(highlight, color);
    }
  }
}

void MainComponent::graphicsTimerCallback(const float secondsSincelastUpdate) {
  if (ui_layer_.connections.isVisible()) {
    updateConnectionIndicators();
  }

  // if (focused_grid_item_ != nullptr) {
  //   updateInspectorModulationIndicators(); // todo - fix
  // }
}

void MainComponent::updateConnectionIndicators() {
  auto modulationConnections = delegate->getModulations();
  for (int i = 0; i < modulationConnections.size(); i++) {
    if (auto mc = dynamic_cast<ConnectionComponent*>(ui_layer_.connections.listBox.getComponentForRowNumber(i))) {
      auto source = delegate->editorRequestsStatusOutput("modulation_amount_" + std::to_string(i + 1));
      mc->indicator.setCurrentValue(source->value()[0]);
    }
  }
}

void MainComponent::mouseDrag(const MouseEvent& event) {
  if (is_adjusting_inspector_ || modulator_drag_mode_ || is_modulator_adjusting_) return;

  block_grid_.add_button_.setAlpha(0);
  if (event.mods.isLeftButtonDown()) {
    resizeSelectionRect(event);
  }
}

void MainComponent::resizeSelectionRect(const juce::MouseEvent& event) {
  if (focused_grid_item_) {
    toggleGridItemSelection(&block_grid_, focused_grid_item_, false);
  }

  auto relative_event = event.getEventRelativeTo(this);
  auto area = Rectangle<int>(relative_event.getMouseDownPosition(), relative_event.getPosition());
  selection_rect_.setBounds(area);
  repaint();

  auto new_selected_items = block_grid_.getItemsInRectangle(area);

  if (new_selected_items == currently_selected_items_) {
    return;
  }

  for (auto new_item : new_selected_items) {
    new_item->setSelected(true);
  }

  for (auto previously_selected_item : currently_selected_items_) {
    bool isnt_in_new_selected = std::find(new_selected_items.begin(), new_selected_items.end(), previously_selected_item) == new_selected_items.end();
    if (isnt_in_new_selected) {
      previously_selected_item->setSelected(false);
    }
  }

  currently_selected_items_ = new_selected_items;
  multiple_selection_ = currently_selected_items_.size() > 1;
}

void MainComponent::updateInspectorModulationIndicators() {
  // if (focused_grid_item_ == nullptr) return;
  // if (focused_grid_item_->getName() != "BlockComponent") return;

  // auto module = delegate->getBlock(focused_grid_item_->index);

  // for (int parameterIndex = 0; parameterIndex < module->parameters.size(); parameterIndex++) {
  //   auto parameter = module->parameters[parameterIndex];

  //   auto range = parameter->audioParameter->getNormalisableRange();
  //   auto value = parameter->audioParameter->getValue();

  //   inspector_.getSliders()[parameterIndex]->setValue(range.convertFrom0to1(value));

  //   auto modulators = parameter->connections;

  //   for (int modulatorIndex = 0; modulatorIndex < parameter->connections.size(); modulatorIndex++) {
  //     auto modulatorValue = delegate->editorRequestsModulatorValue(focused_grid_item_->index, parameterIndex, modulatorIndex);
  //     inspector_.setModulationIndicatorValue(parameterIndex, modulatorIndex, modulatorValue.first, modulatorValue.second);
  //   }
  // }
}

void MainComponent::clear() {
  copied_blocks_ = {};
  currently_selected_items_ = {};
  focused_grid_item_ = nullptr;
  inspector_.setVisible(false);

  for (auto mc : block_grid_.getItems())
    removeChildComponent(mc);

  block_grid_.clear();
  blocks.clear();

  for (auto tab : tab_grid_.getItems())
    removeChildComponent(tab);

  tab_grid_.clear();

  ui_layer_.preset_button_.content.label.setText("empty", dontSendNotification);
  ui_layer_.setConnections(delegate->getModulations());
  ui_layer_.setModulators(delegate->getModulators2());
  column_controls_.reset();
  resetDownFlowingDots();
}

void MainComponent::modulationConnectionBipolarPressed(ConnectionComponent* component, bool bipolar) {
  auto index = ui_layer_.connections.indexOfModulationConnection(component->getParentComponent());
  delegate->editorChangedModulationPolarity(index, bipolar);

  auto connection = delegate->getModulations()[index];

  if (!focused_grid_item_) return;
  auto focused = delegate->getBlock2(focused_grid_item_->index);
  auto target = connection->target;

  if (target->name != focused->name) return;

  // auto parameter = target->parameters_[connection->parameterIndex];

  // for (int m = 0; m < parameter->connections.size(); m++)
  //   if (connection->source->name == parameter->connections[m]->source->name)
  //     inspector.getSliders()[connection->parameterIndex]->setModulatorBipolar(m, bipolar);
}

void MainComponent::connectionDeleted(ConnectionComponent* component) {
  auto connection = delegate->getModulations()[component->row];
  if (focused_grid_item_) {
    auto focused = delegate->getBlock2(focused_grid_item_->index);
    auto target = connection->target;

    if (target->name != focused->name) return;

    // int index = target->parameters[connection->parameterIndex]->getIndexOfConnection(connection);
    // inspector.getSliders()[connection->parameterIndex]->removeIndicator(index);
  }

  delegate->editorDisconnectedModulation(component->row);
  ui_layer_.setConnections(delegate->getModulations());

  if (inspector_.isVisible()) inspector_.setConfiguration(delegate->getBlock2(focused_grid_item_->index));
  for (auto block : blocks) block->setConfig(delegate->getBlock2(block->index), delegate->getModulations());
}

void MainComponent::sliderValueChanged(Slider* slider) {
  bool isModulatorSlider = slider->getName() == "modulatorSlider";
  if (isModulatorSlider) {
    // auto listItemComponent = slider->getParentComponent()->getParentComponent()->getParentComponent()->getParentComponent()->getParentComponent();
    // auto index = ui_layer_.modulators_.listBox.getRowNumberOfComponent(listItemComponent);
    // delegate->editorAdjustedModulator(index, 1, static_cast<float>(slider->getValue()));
  } else {
    auto index = ui_layer_.connections.indexOfModulationConnection(slider->getParentComponent()->getParentComponent());
    auto value = static_cast<float>(slider->getValue());
    delegate->editorChangedModulationMagnitude(index, static_cast<float>(slider->getValue()));
  }
}

// void MainComponent::sliderValueChanged

void MainComponent::loadState(Preset preset) {
  for (auto presetBlock : preset.blocks) {
    auto block = delegate->getBlock2(Index { presetBlock.index.first, presetBlock.index.second });
    spawnBlockComponent(block);
  }

  // for (auto presetTab : preset.tabs) {
  //   auto tab = delegate->getTab(presetTab.column);
  //   spawnTabComponent(tab);
  // }

  for (auto column_control : preset.column_controls) {
    int index = column_control.id.number - 1;
    column_controls_.pan_sliders_[index]->slider.setValue(column_control.parameters["pan"]);
    column_controls_.level_sliders_[index]->slider.setValue(column_control.parameters["level"]);
  }

  ui_layer_.setConnections(delegate->getModulations());
  ui_layer_.setModulators(delegate->getModulators2());
  ui_layer_.preset_button_.content.label.setText(preset.name, dontSendNotification);

  for (auto block : blocks) block->animate();
}

void MainComponent::modulatorIsDragging(ModulatorComponent* modulatorComponent, const MouseEvent& event) {
  updateDotPosition(event.getEventRelativeTo(this).getPosition());

  auto relative_event = event.getEventRelativeTo(this);
  auto component_under_mouse = getComponentAt(relative_event.getPosition());
  if (component_under_mouse && component_under_mouse->getName() == "blocks_core_slider") {
    if (component_under_mouse == last_hovered_slider_) {
      printf("pls pls return\n");
      return;
    }
    last_hovered_slider_ = component_under_mouse;
    if (auto box_slider = dynamic_cast<BoxSlider*>(component_under_mouse->getParentComponent())) {
      delegate->editorConnectedModulation(modulatorComponent->row, box_slider->module_id_.getName(), box_slider->parameter_name_);
      // delegate
    }
    auto parent = component_under_mouse->getParentComponent()->getName().toStdString();
    // printf("Component at point: %s parent: %s\n", component_under_mouse->getName().toStdString().c_str(), parent.c_str());
  } else {
    // printf("No component at point\n");
  }

  if (inspector_.isVisible()) {
    auto relativePosition = event.getEventRelativeTo(&inspector_).getPosition();
    bool draggingInsideInspector = inspector_.contains(relativePosition);

    if (draggingInsideInspector) {
      int sliderIndexUnderMouse = (int)ceilf(relativePosition.getX() / inspector_.sliderWidth);
      if (sliderIndexUnderMouse == previous_slider_under_mouse_) return;

      if (previous_slider_under_mouse_.has_value())
        inspector_.getSliders()[*previous_slider_under_mouse_]->setHighlighted(false);

      previous_slider_under_mouse_ = sliderIndexUnderMouse;

      auto target = getFocusedModule();

      if (target->parameters_[sliderIndexUnderMouse]->modulatable) {
        auto slider = inspector_.getSliders()[sliderIndexUnderMouse];
        slider->setHighlighted(true, modulatorComponent->getColour());
      }
      // } else {
        // if (previousSliderUnderMouse.has_value()) {
        //   inspector.getSliders()[*previousSliderUnderMouse]->setHighlighted(false);
        //   previousSliderUnderMouse = {};
        // }
    }
  }

  // auto pos = event.getEventRelativeTo(&ui_layer_.modulators_.listBox).getPosition();

  // std::cout << "x: " << pos.getX() << " y: " << pos.getY() << std::endl;
  // auto component_under_mouse = getComponentAt

  // bool dragging_inside_modulators = ui_layer_.modulators_.listBox.contains(pos);
  // if (dragging_inside_modulators) {
  //   if (ui_layer_.modulators_.modulators_list_model_.getNumRows() == 0) return;
  //   int itemHeight = ui_layer_.modulators_.listBox.getRowHeight();
  //   int index = (int)ceilf(pos.getY() / itemHeight);
  //   if (auto modulator_at_index = dynamic_cast<ModulatorComponent*>(ui_layer_.modulators_.listBox.getComponentForRowNumber(index))) {
  //     auto component_under_mouse = modulator_at_index->getComponentAt(pos);
  //     // std::cout << "component under mouse: " << component_under_mouse << std::endl;
  //     if (auto slider = dynamic_cast<BoxSlider*>(component_under_mouse)) {
  //       std::cout << "slider name: " << slider << std::endl;
        // auto parameter_name = modulator_at_index->getParameterName(slider->getName().getIntValue());
        // modulatorIsAdjusting(modulatorComponent, parameter_name, slider->getValue());
      // }
      // auto 
      // std::cout << "sup" << std::endl;
      // modulator_at_index->sliders
      // auto relative_pos = event.getEventRelativeTo(modulator_at_index).getPosition();
      // auto slider_at_pos = modulator_at_index->getComponentAt(relative_pos);
      // if (auto slider = dynamic_cast<LabeledSlider*>(slider_at_pos)) {
      //   std::cout << "slider name: " << slider->label.getText() << std::endl;
        // auto parameter_name = modulator_at_index->getParameterName(slider->getName().getIntValue());
        // modulatorIsAdjusting(modulatorComponent, parameter_name, slider->getValue());
      // }
  //   }
  // }
}

std::shared_ptr<model::Module> MainComponent::getFocusedModule() {
  // if (focusedGridItem->grid == &blockGrid)
  return delegate->getBlock2(focused_grid_item_->index);
  // else
  //   return delegate->getTab(focusedGridItem->index.column);
}

void MainComponent::modulatorEndedDrag(ModulatorComponent* modulator_component, const MouseEvent& event) {
  exitModulatorDragMode();

  auto grid_relative_pos = event.getEventRelativeTo(&block_grid_).getPosition();
  auto inspector_relative_pos = event.getEventRelativeTo(&inspector_).getPosition();
  auto modulator_index = ui_layer_.modulators_.listBox.getRowNumberOfComponent(modulator_component->getParentComponent());

  if (block_grid_.contains(grid_relative_pos)) {
    auto index_under_mouse = block_grid_.indexForPoint(grid_relative_pos);

    // if landed on a block
    if (auto block = block_grid_.isSlotTaken(index_under_mouse)) {
      // presentModulationOptionsMenu(modulatorIndex, indexUnderMouse, block); // todo - reimplement
      return;
    }
  } else if (inspector_.contains(inspector_relative_pos)) {
    int parameter_index = (int)ceilf(inspector_relative_pos.getX() / inspector_.sliderWidth);

    auto focused_module = getFocusedModule();
    if (focused_module == nullptr) return;

    auto is_modulatable = focused_module->parameters_[parameter_index]->modulatable;
    if (!is_modulatable) return;

    auto parameter_name = focused_module->getParameterName(parameter_index);
    delegate->editorConnectedModulation(modulator_component->row, focused_module->name, parameter_name);
    ui_layer_.setConnections(delegate->getModulations());
    refreshInspector();
    auto focused_block = block_matrix_[focused_grid_item_->index.column][focused_grid_item_->index.row];
    focused_block->setConfig(focused_module, delegate->getModulations());
    ui_layer_.connections.setVisible(true);
  }
}

void MainComponent::exitModulatorDragMode() {
  setMouseCursor(MouseCursor::NormalCursor);
  dark_background_.setVisible(false);
  cursor.setVisible(false);
  highlightModulatableSliders(false);
  last_hovered_slider_ = nullptr;
}

void MainComponent::presentModulationOptionsMenu(int modulatorIndex, Index& indexUnderMouse, BlockComponent* block) {
  // auto block_model = delegate->getBlock2(indexUnderMouse);
  // PopupMenu modulate_menu = spawnModulationMenu(*block_model);

  // const int chosen_index = 1;
  // bool discarded_menu = chosen_index == 0;
  // if (discarded_menu) return;

  // // delegate->editorConnectedModulation(modulatorIndex, blockModel->name, chosenIndex);
  // ui_layer_.setConnections(delegate->getModulations());

  // if (inspector_.isVisible()) refreshInspector();
}

void MainComponent::enterModulatorDragMode(Colour colour) {
  // dark_background_.setVisible(true);
  cursor.colour = colour;
  cursor.setVisible(true);
  inspector_.setAlwaysOnTop(true);
  cursor.setAlwaysOnTop(true);
  modulator_drag_mode_ = true;
  highlightModulatableSliders(true, colour);
  // ui_layer_.modulators_.mode
}

void MainComponent::modulatorRemoved(ModulatorComponent* component) {
  ui_layer_.modulators_.modulators_list_model_.remove(component->row);
  ui_layer_.modulators_.listBox.updateContent();

  delegate->editorRemovedModulator(component->row);
  ui_layer_.setConnections(delegate->getModulations());

  // if (inspector.isVisible()) inspector.setConfiguration(delegate->getBlock(focusedGridItem->index));
  for (auto block : blocks) block->setConfig(delegate->getBlock2(block->index), delegate->getModulations());
}

void MainComponent::setupPopupMenus() {
  addChildComponent(blocks_popup_);
  blocks_popup_.setModel({ model::block_popup_column_one, model::block_popup_column_two });
  addChildComponent(modulators_popup_);

  modulators_popup_.setModel({ model::modulators });

  addChildComponent(save_popup_);
  addChildComponent(copy_delete_popup_);
  addChildComponent(paste_popup_);
}

void MainComponent::presetButtonClicked() {
  auto presetButtonBounds = ui_layer_.preset_button_.getBounds();
  auto y = presetButtonBounds.getY() + presetButtonBounds.getHeight() + 6;

  auto width = 140;
  auto x = presetButtonBounds.getCentreX() - width / 2;

  int presetCount = delegate->editorRequestsPresetNames().size();
  int desiredHeight = presets_popup_.calculateHeight(presetCount);
  int maxHeight = presets_popup_.calculateHeight(10);
  auto height = std::clamp(desiredHeight, desiredHeight, maxHeight);
  presets_popup_.setBounds(x, y, width, height);

  StringArray presetNames = delegate->editorRequestsPresetNames();
  presets_popup_.setModel({ presetNames });

  showPopup(presets_popup_, [this](Index i) { this->loadPreset(i.row); });
}

void MainComponent::loadPreset(int index) {
  clear();
  auto preset = delegate->editorChangedPreset(index);
  loadState(preset);
  dark_background_.setVisible(false);
}

void MainComponent::visibilityChanged() {
  loadState(delegate->getStateRepresentation());
}

void MainComponent::clickedOnGrid(GridComponent* grid, Index index) {
  if (grid == &block_grid_) return;
  // spawnTabComponent(delegate->editorAddedTab(index.column));
}

void MainComponent::gridItemRemoved(GridComponent* grid, GridItemComponent* item) {
  if (grid == &block_grid_) {
    removeBlock(item);
  }
}

void MainComponent::gridItemRepositioned(GridComponent* grid, GridItemComponent* item, Index oldIndex) {
  if (grid == &block_grid_) {
    block_matrix_[oldIndex.column][oldIndex.row] = nullptr;
    block_matrix_[item->index.column][item->index.row] = static_cast<BlockComponent*>(item);
    delegate->editorRepositionedBlock(oldIndex, item->index);
    // ResetDownFlowingDots();
  } else if (grid == &tab_grid_) {
    delegate->editorRepositionedTab(oldIndex.column, item->index.column);
  }
}

void MainComponent::gridItemLengthChanged(GridComponent* grid, GridItemComponent* item, int length) {
  if (grid == &block_grid_) {
    delegate->editorChangedBlockLength(item->index, length);
  } else if (grid == &tab_grid_) {
    delegate->editorChangedTabLength(item->index.column, length);
  }
}

void MainComponent::gridItemHovered(GridComponent* grid, GridItemComponent* item, Index index) {
  if (grid == &tab_grid_) {
    block_grid_.highlightColumn(index.column, index.column + item->length);
  } else {
    repaint();
  }
}

void MainComponent::gridItemStartedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) {
  if (grid == &tab_grid_) {
    tab_grid_.hideAllItems(true, item);
    // grid_dark_background_.setVisible(true);
  }
}

void MainComponent::gridItemEndedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) {
  if (grid == &tab_grid_) {
    grid_dark_background_.setVisible(false);
    tab_grid_.hideAllItems(false, item);
    block_grid_.reset();
    block_grid_.resetDotsVisibility();
  }
}

void MainComponent::gridItemClicked(GridComponent* grid, GridItemComponent* item, const MouseEvent& event) {
  if (event.mods.isRightButtonDown()) {
    showCopyDeletePopup(event, item);
    return;
  }

  if (multiple_selection_) {
    block_grid_.setAllItemsSelected(false);
    multiple_selection_ = false;
  }
  toggleGridItemSelection(grid, item, !item->isSelected);
}

void MainComponent::showCopyDeletePopup(const juce::MouseEvent& event, GridItemComponent* item) {
  auto grid_relative_event = event.getEventRelativeTo(&block_grid_);
  auto index_under_mouse = block_grid_.indexForPoint(grid_relative_event.getPosition());
  auto selected_indices = block_grid_.getSelectedIndices();
  bool is_index_under_mouse_selected = std::find(selected_indices.begin(), selected_indices.end(), index_under_mouse) != selected_indices.end();
  if (!is_index_under_mouse_selected) {
    block_grid_.setAllItemsSelected(false);
    toggleGridItemSelection(&block_grid_, item, true);
  }

  StringArray model { "copy", "delete" };
  copy_delete_popup_.setModel(model);
  auto this_relative_event = event.getEventRelativeTo(this);
  copy_delete_popup_.setBounds(this_relative_event.getPosition().getX(), this_relative_event.getPosition().getY(), 56, 62);
  showPopup(copy_delete_popup_, [this, item](Index i) {
    if (i.row == 0) {
      copy();
    } else {
      removeSelectedItems();
    }
  });
}

void MainComponent::removeSelectedItems() {
  for (auto index : this->block_grid_.getSelectedIndices()) {
    auto block_component = block_matrix_[index.column][index.row];
    removeBlock(block_component);
  }
  currently_selected_items_ = {};
}

void MainComponent::toggleGridItemSelection(GridComponent* grid, GridItemComponent* item, bool selected) {
  inspector_.setVisible(selected);

  if (selected) {
    bool is_other_module_currently_chosen = focused_grid_item_ != nullptr;
    if (is_other_module_currently_chosen) focused_grid_item_->setSelected(false);
    focused_grid_item_ = item;
    refreshInspector();
  } else {
    focused_grid_item_ = nullptr;
  }

  item->setSelected(selected);
}

void MainComponent::notesStarted(Array<int> notes) {
  tab_grid_.animateGlowIndicators(delegate->editorRequestsActiveColumns());
}

void MainComponent::notesEnded(Array<int> notes) { }

void MainComponent::resetDownFlowingDots() {
  std::set<int> columns_with_blocks;
  for (auto block_component : blocks) {
    auto block_model = delegate->getBlock2(block_component->index);
    if (block_model->id.type != "osc") continue;
    columns_with_blocks.insert(block_component->index.column);
  }

  for (int column = 0; column < GridConfigs::blocks.columns; column++) {
    block_grid_.setDownFlowingHighlight(column, false);
  }

  for (auto column : columns_with_blocks) {
    block_grid_.setDownFlowingHighlight(column, true);
  }
}

void MainComponent::columnControlAdjusted(ColumnControlsContainer::ControlType control, int column, float value) {
  if (control == ColumnControlsContainer::ControlType::level) {
    delegate->editorAdjustedColumn("level", column, value);
  } else if (control == ColumnControlsContainer::ControlType::pan) {
    delegate->editorAdjustedColumn("pan", column, value);
  }
}

void MainComponent::columnControlStartedAdjusting(ColumnControlsContainer::ControlType control, int column) {
  if (control == ColumnControlsContainer::ControlType::level) {
    delegate->editorStartedAdjustingColumn("level", column);
  } else if (control == ColumnControlsContainer::ControlType::pan) {
    delegate->editorStartedAdjustingColumn("pan", column);
  }
}

void MainComponent::columnControlEndedAdjusting(ColumnControlsContainer::ControlType control, int column) {
  if (control == ColumnControlsContainer::ControlType::level) {
    delegate->editorEndedAdjustingColumn("level", column);
  } else if (control == ColumnControlsContainer::ControlType::pan) {
    delegate->editorEndedAdjustingColumn("pan", column);
  }
}

void MainComponent::modulatorGestureChanged(ModulatorComponent* modulatorComponent, std::string parameter_name, bool started) {
  is_modulator_adjusting_ = started;
  auto modulator = delegate->getModulator2(modulatorComponent->row);
  delegate->editorParameterGestureChanged(modulator->name, parameter_name, started);
}

void MainComponent::copy() {
  copied_blocks_.clear();
  auto items = block_grid_.getItems();
  for (auto item : items) {
    if (item->isSelected) {
      auto block = delegate->getBlock2(item->index);
      copied_blocks_.push_back(*block);
    }
  }
}

bool MainComponent::keyPressed(const KeyPress& key, Component* originatingComponent) {
  // todo - make this work
  if (key.getKeyCode() == juce::KeyPress::backspaceKey) {
    for (auto item : block_grid_.getItems()) {
      if (item->isSelected) {
        auto block = block_matrix_[item->index.column][item->index.row];
        removeBlock(block);
      }
    }
  }
  return true;
}