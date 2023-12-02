#include "MainComponent.h"
#include "ModuleProcessorFactory.h"
#include "model/ModuleFactory.h"
#include "settings/GridConfigs.h"
#include "gui/ThemeManager.h"
#include "settings/UserSettings.h"
#include "module_new.h"

MainComponent::MainComponent(juce::MidiKeyboardState& keyboard_state, Delegate* delegate): delegate(delegate), ui_layer_(keyboard_state, this), tab_grid_(GridConfigs::tab), block_grid_(GridConfigs::blocks) {
  setWantsKeyboardFocus(false);

  setLookAndFeel(&blocks_laf_);
  setupPopupMenus();
  setupInspector();
  setupUI();
  setupListeners();
  addAndMakeVisible(block_placeholder_);
  addChildComponent(cursor, 1000);
  setupPresetMenu();
  setupDarkBackground(&grid_dark_background_, 3);
  addChildComponent(ui_layer_.modulators_);
  setupTabGrid();
  setupBlockGrid();
  setupDarkBackground(&dark_background_, -1);

  note_logger_.listener = this;
  ThemeManager::shared()->set(UserSettings::shared()->getInt("theme", 0));
  //
  // auto block = addBlock(0, { 0, 0 });
  // spawnBlockComponent(block);

  // addModulator(Model::Types::lfo);
}

void MainComponent::updateDotPosition(const Point<int> position) {
  cursor.setBounds(position.getX(), position.getY(), cursor.size, cursor.size);
}

void MainComponent::modulatorStartedAdjusting(ModulatorComponent* modulatorComponent, int index) {
  // delegate->editorParameterGestureChanged(delegate->getModulator(modulatorComponent->row)->name, index, true);
}

void MainComponent::modulatorEndedAdjusting(ModulatorComponent* modulatorComponent, int index) {
  // delegate->editorParameterGestureChanged(delegate->getModulator(modulatorComponent->row)->name, index, false);
}

void MainComponent::modulatorIsAdjusting(ModulatorComponent* component, int parameter, float value) {
  delegate->editorAdjustedModulator(parameter, component->row, value);
}

void MainComponent::paint(juce::Graphics& g) {
  g.fillAll(ThemeManager::shared()->getCurrent().background);
}

void MainComponent::inspectorGestureChanged(int index, bool started) {
  // std::shared_ptr<Module> focusedModule;

  // auto isTab = tabGrid.containsItem(focusedGridItem);
  // if (isTab) {
  //   focusedModule = delegate->getTab(focusedGridItem->index.column);
  // } else {
  //   focusedModule = delegate->getBlock(focusedGridItem->index);
  // }

  // delegate->editorParameterGestureChanged(focusedModule->name, index, started);
}

void MainComponent::changeModulePainter(int value) {
  auto castedBlock = static_cast<BlockComponent*>(focused_grid_item_);
  castedBlock->getPainter()->setWaveformType(static_cast<OscillatorPainter::WaveformType>(value));
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

void MainComponent::darkBackgroundClicked(Component* component) {
  component->setVisible(false);
  Component* popups[] = { &blocks_popup_, &modualtors_popup_, &presets_popup_, &save_popup_ };

  for (auto popup : popups) {
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
  addAndMakeVisible(tab_grid_, 2);

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

    delegate->editorSavedPreset(name);
    ui_layer_.preset_button_.setStrings(delegate->editorRequestsPresetNames());
    save_popup_.setVisible(false);
  };

  ui_layer_.newPresetButton->on_click_ = [this]() {
    DBG("changed preset");
    delegate->editorChangedPreset(-1);
    return;
    clear();
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
  ui_layer_.modulators_.on_added_modulator_ = [this](int index) { addModulator(Model::Types::all[index]); };
  ui_layer_.preset_button_.on_click_ = [this]() { presetButtonClicked(); };
  ui_layer_.theme_button_->on_click_ = [this]() {
    UserSettings::shared()->set("theme", ThemeManager::shared()->next());
    repaint();
  };
}

void MainComponent::addModulator(Model::Type code) {
  auto module = delegate->editorAddedModulator2(code);
  if (module == nullptr) return;
  ui_layer_.setModulators(delegate->getModulators2());
}

void MainComponent::resized() {
  ui_layer_.setBounds(getLocalBounds());
  resizeGrid();
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
}

void MainComponent::resizeTabContainer() {
  int height = Constants::tabHeight;
  int verticalSpacing = 19;
  tab_grid_.setBounds(block_grid_.getX(), block_grid_.getY() - height - verticalSpacing, block_grid_.getWidth(), height);
}

void MainComponent::mouseDown(const MouseEvent& event) {
  if (auto* module = dynamic_cast<GridComponent*>(event.eventComponent))
    return;
}

void MainComponent::mouseUp(const MouseEvent& event) {
  setMouseCursor(MouseCursor::NormalCursor);

  if (dark_background_.isVisible()) {
    dark_background_.setVisible(false);
  }

  if (modulator_drag_mode_) {
    previous_slider_under_mouse_ = {};
    modulator_drag_mode_ = false;
    return;
  }

  auto mousePosition = event.getEventRelativeTo(&block_grid_).getPosition();
  auto componentName = event.eventComponent->getName();

  if (componentName == "ModulatorsPlusButton") {
    auto position = event.eventComponent->getPosition() + ui_layer_.modulators_.getPosition();
    modualtors_popup_.setBounds(position.getX(), position.getY(), 72, 54);
    showPopupAt(modualtors_popup_, [this](Index i) { this->clickOnModulatorsPopup(i); });
  } else if (componentName == "PresetMainButton") {
    auto componentY = event.eventComponent->getPosition().getY();
    auto point = event.eventComponent->getPosition().withY(componentY + 8);
    showPopupAt(presets_popup_, [this](Index i) { this->clickOnModulatorsPopup(i); });
  }

  if (block_grid_.contains(mousePosition)) {
    auto index = block_grid_.indexForPoint(mousePosition); // todo fix 
    clickOnGrid(index);
  }

  cursor.setVisible(false);
}

void MainComponent::clickOnModulatorsPopup(Index index) {
  auto code = index.row == 0 ? Types::lfo : Types::adsr;
  addModulator(code);
  dark_background_.setVisible(false);
}

void MainComponent::clickOnGrid(Index& index) {
  if (blocks_popup_.isVisible()) {
    blocks_popup_.setVisible(false);
  } else {
    ShowBlocksPopup(index);
  }
}

void MainComponent::resizeGrid() {
  using namespace Constants;
  const float gridWidth = columns * moduleWidth + (columns - 1) * moduleSpacing + gridEdgeSpacing * 2;
  const float gridHeight = rows * moduleHeight + (rows - 1) * (moduleSpacing)+gridEdgeSpacing * 2;
  const float x = (getLocalBounds().getWidth() - gridWidth) / 2;

  int gridTopOffset = 135;
  block_grid_.setBounds(x, getLocalBounds().getY() + gridTopOffset, gridWidth, gridHeight);

  for (auto moduleComponent : block_grid_.getItems())
    block_grid_.resnapBlock(moduleComponent);
}

void MainComponent::ShowBlocksPopup(Index index) {
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

  showPopupAt(blocks_popup_, blockSelectionCompletion);
}

void MainComponent::showPopupAt(ButtonGridPopup& popup, std::function<void(Index)> callback) {
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

  switch (code) { // the first row's 5 codes are reserved for different wave types in the block selection menu popup
  case 0:
  case 1:
  case 2:
  case 3:
  case 4: {
    block = delegate->editorAddedBlock2(Model::Types::osc, index);
    DBG(block->parameters_.size());

    // if (block == nullptr) return nullptr; // todo - grey out the button in the block selection popup if the block is not available
    // auto range = block->parameters[0]->audioParameter->getNormalisableRange();
    // block->parameters[0]->audioParameter->setValue(range.convertTo0to1(code));
    break;
  }
  case 5: block = delegate->editorAddedBlock2(Model::Types::filter, index); break;
    // case 5: block = delegate->editorAddedBlock(Model::Types::filter, index); break;
    // case 6: block = delegate->editorAddedBlock(Model::Types::reverb, index); break;
    // case 7: block = delegate->editorAddedBlock(Model::Types::delay, index); break;
    // case 8: block = delegate->editorAddedBlock(Model::Types::drive, index); break;
    // case 9: block = delegate->editorAddedBlock(Model::Types::mixer, index); break;
  default: break;
  }

  return block;
}

void MainComponent::setupInspector() {
  inspector_.addMouseListener(this, true);
  inspector_.delegate = this;
  addChildComponent(inspector_);
}

void MainComponent::ResizeInspector() {
  auto gridY = block_grid_.getY() + block_grid_.getHeight();
  auto gridCenterX = block_grid_.getX() + block_grid_.getWidth() / 2;
  int inspectorX = gridCenterX - inspector_.calculateWidth() / 2;
  inspector_.setBounds(inspectorX, gridY + 60, 1, 220);
}

void MainComponent::DismissPopup(ButtonGridPopup& popup) {
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
  block_grid_.ResetDotsVisibility();

  delegate->editorRemovedBlock(index);
  ui_layer_.setConnections(delegate->getModulations());
  ResetDownFlowingDots();
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
    // auto module = delegate->getBlock(moduleIndex);
    delegate->editorAdjustedBlock(moduleIndex, sliderIndex, value);
    // updateModuleComponentVisuals(sliderIndex, value, static_cast<std::shared_ptr<Block>>(module));
  }
}

void MainComponent::updateModuleComponentVisuals(int sliderIndex, float value, std::shared_ptr<Block> block) {
  if (block->id.type == Model::Types::osc) {
    switch (OscillatorModule::Parameters(sliderIndex)) {
    case OscillatorModule::pWave:
      changeModulePainter((int)value);
      break;
    case OscillatorModule::pUnison: {
      auto module_component = block_matrix_[block->index.row][block->index.column];
      if (auto painter = module_component->getPainter())
        painter->setUnison(static_cast<int>(value));
      break;
    }
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

PopupMenu MainComponent::spawnModulationMenu(Module& victim) {
  PopupMenu modulateMenu;

  modulateMenu.setLookAndFeel(&blocks_laf_);

  for (int i = 0; i < victim.parameters.size(); i++)
    modulateMenu.addItem(i + 1, victim.parameters[i]->id);

  return modulateMenu;
}

void MainComponent::spawnBlockComponent(std::shared_ptr<model::Block> block) {
  auto blockComponent = BlockComponent::create(block);

  blocks.add(blockComponent);
  block_grid_.addItem(blockComponent, block->index, true);
  block_matrix_[block->index.row][block->index.column] = blockComponent;
  addAndMakeVisible(blockComponent, 1000);
  cursor.setAlwaysOnTop(true);
  if (block->length > 1) block_grid_.setItemLength(blockComponent, block->length);
  // blockComponent->setConfig(block);
  // ResetDownFlowingDots();
}

void MainComponent::spawnTabComponent(std::shared_ptr<Tab> tab) {
  auto tabComponent = TabComponent::create(*tab, &tab_grid_);
  addAndMakeVisible(tabComponent, 5);
  tabComponent->toFront(false);
  tab_grid_.addItem(tabComponent, { 0, tab->column }, true);
  if (tab->length > 1) tab_grid_.setItemLength(tabComponent, tab->length);

  for (auto glowIndicator : tab_grid_.glowIndicators)
    glowIndicator->toFront(false);
}

void MainComponent::graphicsTimerCallback(const float secondsSincelastUpdate) {
  return;
  auto currently_playing_notes = delegate->editorRequestsCurrentlyPlayingNotes();
  note_logger_.log(currently_playing_notes);

  if (ui_layer_.connections.isVisible()) {
    auto modulationConnections = delegate->getModulations();

    for (int i = 0; i < modulationConnections.size(); i++) {
      if (auto mc = dynamic_cast<ConnectionComponent*>(ui_layer_.connections.listBox.getComponentForRowNumber(i))) {
        auto value = delegate->editorRequestsModulatorValue(i);
        mc->indicator.setMagnitude(value.second, false);
        mc->indicator.setCurrentValue(value.first);
        mc->indicator.repaint();
      }
    }
  }

  if (focused_grid_item_ != nullptr) {
    updateInspectorModulationIndicators(); // todo - fix
  }
}

void MainComponent::mouseMove(const MouseEvent& event) {
  this->current_mouse_position_ = event.getEventRelativeTo(this).getPosition();
}

void MainComponent::updateInspectorModulationIndicators() {
  if (focused_grid_item_ == nullptr) return;
  if (focused_grid_item_->getName() != "BlockComponent") return;

  auto module = delegate->getBlock(focused_grid_item_->index);

  for (int parameterIndex = 0; parameterIndex < module->parameters.size(); parameterIndex++) {
    auto parameter = module->parameters[parameterIndex];

    auto range = parameter->audioParameter->getNormalisableRange();
    auto value = parameter->audioParameter->getValue();

    inspector_.getSliders()[parameterIndex]->setValue(range.convertFrom0to1(value));

    auto modulators = parameter->connections;

    for (int modulatorIndex = 0; modulatorIndex < parameter->connections.size(); modulatorIndex++) {
      auto modulatorValue = delegate->editorRequestsModulatorValue(focused_grid_item_->index, parameterIndex, modulatorIndex);
      inspector_.setModulationIndicatorValue(parameterIndex, modulatorIndex, modulatorValue.first, modulatorValue.second);
    }
  }
}

void MainComponent::clear() {
  focused_grid_item_ = nullptr;
  inspector_.setVisible(false);

  for (auto mc : block_grid_.getItems())
    removeChildComponent(mc);

  block_grid_.clear();
  blocks.clear();

  for (auto tab : tab_grid_.getItems())
    removeChildComponent(tab);

  tab_grid_.clear();

  ui_layer_.preset_button_.label.setText("empty", dontSendNotification);
  ui_layer_.setConnections(delegate->getModulations());
  ui_layer_.setModulators(delegate->getModulators2());
  ResetDownFlowingDots();
}

void MainComponent::modulationConnectionBipolarPressed(ConnectionComponent* component, bool bipolar) {
  auto index = ui_layer_.connections.indexOfModulationConnection(component->getParentComponent());
  delegate->editorChangedModulationPolarity(index, bipolar);

  auto connection = delegate->getModulations()[index];

  if (!focused_grid_item_) return;
  auto focused = delegate->getBlock(focused_grid_item_->index);
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
    auto focused = delegate->getBlock(focused_grid_item_->index);
    auto target = connection->target;

    if (target->name != focused->name) return;

    // int index = target->parameters[connection->parameterIndex]->getIndexOfConnection(connection);
    // inspector.getSliders()[connection->parameterIndex]->removeIndicator(index);
  }

  delegate->editorDisconnectedModulation(component->row);
  ui_layer_.setConnections(delegate->getModulations());

  if (inspector_.isVisible()) inspector_.setConfiguration(delegate->getBlock2(focused_grid_item_->index));
  for (auto block : blocks) block->setConfig(delegate->getBlock(block->index));
}

void MainComponent::sliderValueChanged(Slider* slider) {
  bool isModulatorSlider = slider->getName() == "modulatorSlider";
  if (isModulatorSlider) {
    auto listItemComponent = slider->getParentComponent()->getParentComponent()->getParentComponent()->getParentComponent()->getParentComponent();
    auto index = ui_layer_.modulators_.listBox.getRowNumberOfComponent(listItemComponent);
    delegate->editorAdjustedModulator(index, 1, static_cast<float>(slider->getValue()));
  } else {
    auto index = ui_layer_.connections.indexOfModulationConnection(slider->getParentComponent()->getParentComponent());
    delegate->editorChangedModulationMagnitude(index, static_cast<float>(slider->getValue()));
  }
}

void MainComponent::loadState(PresetInfo preset) {
  for (auto presetBlock : preset.blocks) {
    auto block = delegate->getBlock(Index { presetBlock.index.first, presetBlock.index.second });
    // spawnBlockComponent(block);
  }

  for (auto presetTab : preset.tabs) {
    auto tab = delegate->getTab(presetTab.column);
    spawnTabComponent(tab);
  }

  ui_layer_.setConnections(delegate->getModulations());
  // uiLayer.setModulators(delegate->getModulators());
  ui_layer_.preset_button_.content.label.setText(preset.name, dontSendNotification);

  for (auto block : blocks) block->animate();
}

void MainComponent::modulatorIsDragging(ModulatorComponent* modulatorComponent, const MouseEvent& event) {
  updateDotPosition(event.getEventRelativeTo(this).getPosition());

  if (!inspector_.isVisible()) return;

  auto relativePosition = event.getEventRelativeTo(&inspector_).getPosition();
  bool draggingInsideInspector = inspector_.contains(relativePosition);

  if (draggingInsideInspector) {
    int sliderIndexUnderMouse = (int)ceilf(relativePosition.getX() / inspector_.sliderWidth);
    if (sliderIndexUnderMouse == previous_slider_under_mouse_) return;

    if (previous_slider_under_mouse_.has_value())
      inspector_.getSliders()[*previous_slider_under_mouse_]->setHighlighted(false);

    previous_slider_under_mouse_ = sliderIndexUnderMouse;

    auto victim = getFocusedModule();

    // if (victim->parameters[sliderIndexUnderMouse]->isModulatable) {
      auto slider = inspector_.getSliders()[sliderIndexUnderMouse];
      slider->setHighlighted(true, modulatorComponent->getColour());
    // }
  // } else {
    // if (previousSliderUnderMouse.has_value()) {
    //   inspector.getSliders()[*previousSliderUnderMouse]->setHighlighted(false);
    //   previousSliderUnderMouse = {};
    // }
  }
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

    // auto isModulatable = focusedModule->parameters[parameterIndex]->isModulatable;
    // if (!isModulatable) return;
    auto parameter_name = focused_module->parameters_[parameter_index]->name;
    delegate->editorConnectedModulation(modulator_component->row, focused_module->name, parameter_name);
    ui_layer_.setConnections(delegate->getModulations());
    refreshInspector();

    // auto modulator = delegate->getModulator(modulatorIndex);
    auto focused_block = block_matrix_[focused_grid_item_->index.row][focused_grid_item_->index.column];

    // focusedBlock->setConfig(focusedModule);
  }
}

void MainComponent::exitModulatorDragMode() {
  setMouseCursor(MouseCursor::NormalCursor);
  dark_background_.setVisible(false);
  cursor.setVisible(false);
}

void MainComponent::presentModulationOptionsMenu(int modulatorIndex, Index& indexUnderMouse, BlockComponent* block) {
  auto block_model = delegate->getBlock(indexUnderMouse);
  PopupMenu modulate_menu = spawnModulationMenu(*block_model);

  const int chosen_index = 1;
  bool discarded_menu = chosen_index == 0;
  if (discarded_menu) return;

  // delegate->editorConnectedModulation(modulatorIndex, blockModel->name, chosenIndex);
  ui_layer_.setConnections(delegate->getModulations());

  if (inspector_.isVisible()) refreshInspector();
}

void MainComponent::enterModulatorDragMode(Colour colour) {
  dark_background_.setVisible(true);
  cursor.colour = colour;
  cursor.setVisible(true);
  inspector_.setAlwaysOnTop(true);
  cursor.setAlwaysOnTop(true);
  modulator_drag_mode_ = true;
}

void MainComponent::modulatorRemoved(ModulatorComponent* component) {
  ui_layer_.modulators_.modulators_list_model_.remove(component->row);
  ui_layer_.modulators_.listBox.updateContent();

  delegate->editorRemovedModulator(component->row);
  ui_layer_.setConnections(delegate->getModulations());

  // if (inspector.isVisible()) inspector.setConfiguration(delegate->getBlock(focusedGridItem->index));
  for (auto block : blocks) block->setConfig(delegate->getBlock(block->index));
}

void MainComponent::setupPopupMenus() {
  addChildComponent(blocks_popup_);
  blocks_popup_.setModel({ waveforms, effects });

  addChildComponent(modualtors_popup_);
  modualtors_popup_.setModel(modulators);

  addChildComponent(save_popup_);
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

  auto presetNames = delegate->editorRequestsPresetNames();
  presets_popup_.setModel({ presetNames });

  showPopupAt(presets_popup_, [this](Index i) { this->loadPreset(i.row); });
}

void MainComponent::loadPreset(int index) {
  clear();
  auto preset = delegate->editorChangedPreset(index);
  loadState(preset);
  dark_background_.setVisible(false);
}

void MainComponent::visibilityChanged() {
  // loadState(delegate->getStateRepresentation());
}

void MainComponent::clickedOnGrid(GridComponent* grid, Index index) {
  if (grid == &block_grid_) return;
  spawnTabComponent(delegate->editorAddedTab(index.column));
}

void MainComponent::gridItemRemoved(GridComponent* grid, GridItemComponent* item) {
  if (grid == &block_grid_) {
    removeBlock(item);
  }
}

void MainComponent::gridItemRepositioned(GridComponent* grid, GridItemComponent* item, Index oldIndex) {
  if (grid == &block_grid_) {
    block_matrix_[oldIndex.row][oldIndex.column] = nullptr;
    block_matrix_[item->index.row][item->index.column] = static_cast<BlockComponent*>(item);
    delegate->editorRepositionedBlock(oldIndex, item->index);
    ResetDownFlowingDots();
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
  }
}

void MainComponent::gridItemStartedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) {
  if (grid == &tab_grid_) {
    tab_grid_.hideAllItems(true, item);
    grid_dark_background_.setVisible(true);
  }
}

void MainComponent::gridItemEndedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) {
  if (grid == &tab_grid_) {
    grid_dark_background_.setVisible(false);
    tab_grid_.hideAllItems(false, item);
    block_grid_.reset();
    block_grid_.ResetDotsVisibility();
  }
}

void MainComponent::gridItemClicked(GridComponent* grid, GridItemComponent* item, const MouseEvent& event) {
  if (event.mods.isRightButtonDown() && grid == &tab_grid_) {
    removeTab(item);
    return;
  }

  toggleGridItemSelection(grid, item, !item->isSelected);
}

void MainComponent::toggleGridItemSelection(GridComponent* grid, GridItemComponent* item, bool selected) {
  grid_dark_background_.setVisible(selected);
  inspector_.setVisible(selected);

  if (selected) {
    item->setHidden(false);
    bool is_other_module_currently_chosen = focused_grid_item_ != nullptr;
    if (is_other_module_currently_chosen) focused_grid_item_->setSelected(false);
    focused_grid_item_ = item;
    refreshInspector();
  } else {
    focused_grid_item_ = nullptr;
  }

  item->setSelected(selected);
  block_grid_.hideAllItems(selected, item);
  tab_grid_.hideAllItems(selected, item);
}

void MainComponent::notesStarted(Array<int> notes) {
  tab_grid_.animateGlowIndicators(delegate->editorRequestsActiveColumns());
}

void MainComponent::notesEnded(Array<int> notes) { }

void MainComponent::ResetDownFlowingDots() {
  std::set<int> columns_with_blocks;
  for (auto block_component : blocks) {
    auto block_model = delegate->getBlock(block_component->index);
    if (block_model->id.type != Model::Types::osc) continue;
    columns_with_blocks.insert(block_component->index.column);
  }

  for (int column = 0; column < GridConfigs::blocks.columns; column++) {
    block_grid_.setDownFlowingHighlight(column, false);
  }

  for (auto column : columns_with_blocks) {
    block_grid_.setDownFlowingHighlight(column, true);
  }
}
