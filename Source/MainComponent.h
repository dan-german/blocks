#pragma once

#include "gui/InspectorComponent.h"
#include "model/Module.h"
#include "gui/BlocksLookAndFeel.h"
#include "model/Modulation.h"
#include "gui/ColourPool.h"
#include "gui/UILayer.h"
#include "model/PresetInfo.h"
#include "gui/GraphicsTimer.h"
#include "gui/ModulatorComponent.h"
#include "gui/Cursor.h"
#include "gui/BlockPlaceholder.h"
#include "model/ModelConstants.h"
#include "gui/TabContainerComponent.h"
#include "gui/TabComponent.h"
#include "gui/BlockGridComponent.h"
#include "gui/Tab.h"
#include "model/NoteLogger.h"
#include "module_new.h"
#include "connection.h"
#include "vital/synthesis/framework/synth_module.h"
#include "gui/column_controls_container.h"
#include "selection_rect.h"

using Modulation = Model::Modulation;
using Block = Model::Block;
using Tab = Model::Tab;

class MainComponent final: public Component,
  InspectorComponent::Listener,
  Slider::Listener,
  ConnectionComponent::Listener,
  ModulatorComponent::Listener,
  GridComponent::Listener,
  NoteLogger::Listener,
  ColumnControlsContainer::Listener {
public:
  struct Delegate;
  Delegate* delegate;
  MainComponent(juce::MidiKeyboardState& keyboard_state, Delegate* delegate);
  ~MainComponent() override;
  void mouseMove(const MouseEvent& event) override;

  BlocksLookAndFeel blocks_laf_;
  UILayer ui_layer_;
  void loadState(Preset preset);
  void visibilityChanged() override;
protected:
  void paint(juce::Graphics&) override;
  void resized() override;
  void resizeColumnControls();

  // MouseListener
  void mouseDown(const MouseEvent& event) override;
  void mouseUp(const MouseEvent& event) override;
private:
  DarkBackground dark_background_;
  DarkBackground grid_dark_background_;
  BlockGridComponent block_grid_;
  TabContainerComponent tab_grid_;
  InspectorComponent inspector_;
  SavePopup save_popup_;
  GraphicsTimer timer_;
  ColumnControlsContainer column_controls_;
  SelectionRect selection_rect_;

  Array<BlockComponent*> blocks;
  GridItemComponent* focused_grid_item_ = nullptr;

  BlockComponent* block_matrix_[Constants::columns][Constants::rows];
  Cursor cursor;
  std::optional<int> previous_slider_under_mouse_ = {};
  BlockPlaceholder block_placeholder_;
  bool is_blocks_popup_visible_ = false;
  bool modulator_drag_mode_ = false;
  ButtonGridPopup blocks_popup_;
  ButtonGridPopup modualtors_popup_;
  ButtonGridPopup presets_popup_;
  NoteLogger note_logger_;

  void setupInspector();
  void clear();
  void resizeGrid();
  void resizeTabContainer();
  void ResizeInspector();
  void clickOnModulatorsPopup(Index index);
  void loadPreset(int index);
  void setupPopupMenus();
  void darkBackgroundClicked(Component* darkBackground);
  void setupDarkBackground(DarkBackground* component, int layer);
  void resetDownFlowingDots();

  void toggleGridItemSelection(GridComponent* grid, GridItemComponent* item, bool selected);
  void ShowBlocksPopup(Index index);
  std::shared_ptr<model::Block> addBlock(int code, Index index);
  void removeBlock(GridItemComponent* block);
  void removeTab(GridItemComponent* tab);
  std::shared_ptr<model::Module> getFocusedModule();

  // Grid Listener
  void clickedOnGrid(GridComponent* grid, Index index) override;
  void gridItemRemoved(GridComponent* grid, GridItemComponent* item) override;
  void gridItemRepositioned(GridComponent* grid, GridItemComponent* item, Index oldIndex) override;
  void gridItemLengthChanged(GridComponent* grid, GridItemComponent* item, int length) override;
  void gridItemHovered(GridComponent* grid, GridItemComponent* item, Index index) override;
  void gridItemClicked(GridComponent* grid, GridItemComponent* item, const MouseEvent& event) override;
  void gridItemStartedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) override;
  void gridItemEndedDrag(GridComponent* grid, GridItemComponent* item, const MouseEvent& mouseEvent) override;

  void modulationConnectionBipolarPressed(ConnectionComponent* component, bool bipolar) override;
  // NoteLogger::Listener
  void notesStarted(Array<int> notes) override;
  void notesEnded(Array<int> notes) override;

  // InspectorComponent::Listener
  void inspectorChangedParameter(int sliderIndex, float value) override;
  void inspectorGestureChanged(std::string parameter_name, bool started) override;

  void DismissPopup(ButtonGridPopup& popup);
  void spawnBlockComponent(std::shared_ptr<model::Block> block);
  void spawnTabComponent(std::shared_ptr<Tab> tab);
  void graphicsTimerCallback(const float secondsSinceLastUpdate);
  void changeModulePainter(int value);
  PopupMenu spawnModulationMenu(Module& victim);
  void showPopupAt(ButtonGridPopup& popupWrapper, std::function<void(Index)> callback);
  void updateInspectorModulationIndicators();
  void handleModuleLandedOnInspector(BlockComponent* moduleComponent, const Point<int>& inspectorRelativePosition);
  void refreshInspector();
  void setupUI();
  void setupBlockGrid();
  void setupTabGrid();
  void updateModuleComponentVisuals(int sliderIndex, float value, std::shared_ptr<model::Module> block);
  void setupListeners();
  void clickOnGrid(Index& index);

  void modulatorEndedDrag(ModulatorComponent* modulatorComponent, const MouseEvent& event) override;
  void modulatorIsDragging(ModulatorComponent* modulatorComponent, const MouseEvent& event) override;
  void modulatorStartedDrag(ModulatorComponent* component, const MouseEvent& event) override;
  void modulatorStartedAdjusting(ModulatorComponent* modulatorComponent, int index) override;
  void modulatorEndedAdjusting(ModulatorComponent* modulatorComponent, int index) override;
  void modulatorIsAdjusting(ModulatorComponent* component, std::string parameter_name, float value) override;
  void modulatorGestureChanged(ModulatorComponent* modulatorComponent, std::string parameter_name, bool started) override;

  void presentModulationOptionsMenu(int modulatorIndex, Index& indexUnderMouse, BlockComponent* block);
  void updateDotPosition(const Point<int> position);
  void addModulator(Model::Type code);
  void enterModulatorDragMode(Colour colour);
  void connectionDeleted(ConnectionComponent* component) override;

  void exitModulatorDragMode();
  void modulatorRemoved(ModulatorComponent* component) override;
  void setupPresetMenu();
  void presetButtonClicked();

  void sliderValueChanged(Slider* slider) override;
  // void sliderDragStarted(Slider* slider) override;
  // void sliderDragEnded(Slider* slider) override;

  void columnControlAdjusted(ColumnControlsContainer::ControlType control, int column, float value) override;
  void columnControlStartedAdjusting(ColumnControlsContainer::ControlType control, int column) override;
  void columnControlEndedAdjusting(ColumnControlsContainer::ControlType control, int column) override;

  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};

struct MainComponent::Delegate {
  virtual std::shared_ptr<Tab> editorAddedTab(int column) = 0;
  virtual void editorRepositionedTab(int oldColumn, int newColumn) = 0;
  virtual void editorChangedTabLength(int column, int times) = 0;
  virtual void editorRemovedTab(int column) = 0;

  virtual void editorRepositionedBlock(Index oldIndex, Index newIndex) = 0;
  virtual void editorRemovedBlock(Index index) = 0;
  virtual void editorAdjustedBlock(Index moduleIndex, int parameterIndex, float value) = 0;
  virtual void editorAdjustedTab(int column, int parameterIndex, float value) = 0;
  virtual void editorAdjustedModulator(std::string parameter_name, int index, float value) = 0;
  virtual void editorConnectedModulation(int modulator_index, std::string target_name, std::string parameter) = 0;

  virtual void editorStartedAdjustingColumn(std::string control, int column) = 0;
  virtual void editorEndedAdjustingColumn(std::string control, int column) = 0;
  virtual void editorAdjustedColumn(std::string contorl, int column, float value) = 0;

  virtual void editorChangedModulationMagnitude(int modulationConnectionIndex, float magnitude) = 0;
  virtual void editorChangedModulationPolarity(int index, bool bipolar) = 0;
  virtual void editorDisconnectedModulation(int index) = 0;
  virtual void editorParameterGestureChanged(std::string module_name, std::string paramter_name, bool started) = 0;
  virtual void editorRemovedModulator(int index) = 0;
  virtual void editorChangedBlockLength(Index index, int times) = 0;
  virtual void editorSavedPreset(std::string name) = 0;
  virtual std::optional<Preset> editorNavigatedPreset(bool next) = 0;

  virtual Array<MPENote> editorRequestsCurrentlyPlayingNotes() = 0;
  virtual StringArray editorRequestsPresetNames() = 0;
  virtual Preset editorChangedPreset(int index) = 0;
  virtual Preset getStateRepresentation() = 0;

  virtual std::pair<float, float> editorRequestsModulatorValue(Index moduleIndex, int parameterIndex, int modulatorIndex) = 0;
  virtual std::pair<float, float> editorRequestsModulatorValue(int modulationConnectionIndex) = 0;
  virtual std::shared_ptr<Tab> getTab(int column) = 0;
  virtual std::shared_ptr<Module> getModulator(int index) = 0;
  virtual std::shared_ptr<model::Module> getModulator2(int index) = 0;
  virtual std::shared_ptr<Module> editorAddedModulator(Model::Type code) = 0;
  virtual std::shared_ptr<model::Module> editorAddedModulator2(Model::Type code) = 0;
  virtual std::shared_ptr<Block> getBlock(Index index) = 0;
  virtual std::shared_ptr<model::Block> getBlock2(Index index) = 0;
  virtual std::shared_ptr<model::Block> editorAddedBlock2(Model::Type code, Index index) = 0;
  virtual std::shared_ptr<Block> editorAddedBlock(Model::Type code, Index index) = 0;
  virtual Array<int> editorRequestsActiveColumns() = 0;
  virtual Array<std::shared_ptr<Module>> getModulators() = 0;
  virtual std::vector<std::shared_ptr<model::Module>> getModulators2() = 0;
  virtual std::vector<std::shared_ptr<model::Connection>> getModulations() = 0;
  virtual Array<std::shared_ptr<Modulation>> getConnectionsOfSource(std::shared_ptr<Module> source) = 0;
  virtual const vital::StatusOutput* editorRequestsStatusOutput(std::string name) = 0;

  virtual ~Delegate() = default;
};