#include "modulation_state_manager.h"

using namespace gui;

ModulationStateManager::ModulationStateManager(Listener* listener, Component& base_component): listener_(listener), base_component_(base_component) {}

void ModulationStateManager::process(ModulatorComponent* modulator_component, const juce::MouseEvent& event) {
  auto component_under_mouse = base_component_.getComponentAt(event.getEventRelativeTo(&base_component_).getPosition());
  if (!component_under_mouse || component_under_mouse->getName() != "blocks_core_slider") {
    handleNoComponentFound(modulator_component);
    return;
  }
  if (component_under_mouse == last_hovered_slider_) return;
  handleModulationHoverEnd(modulator_component);
  last_hovered_slider_ = component_under_mouse;
  handleModulationHover(modulator_component);
}

void ModulationStateManager::handleNoComponentFound(const ModulatorComponent* modulator_component) {
  handleModulationHoverEnd(modulator_component);
  last_hovered_slider_ = nullptr;
}

void ModulationStateManager::handleModulationHoverEnd(const ModulatorComponent* modulator_component) {
  if (last_hovered_slider_ && is_currently_modulating_slider_) {
    auto last_slider = dynamic_cast<BlocksSlider*>(last_hovered_slider_->getParentComponent()->getParentComponent());
    if (last_slider && last_slider->modulatable) {
      listener_->unhovered(last_slider, modulator_component);
      is_currently_modulating_slider_ = false;
    }
  }
}

void ModulationStateManager::handleModulationHover(const ModulatorComponent* modulator_component) {
  auto slider = dynamic_cast<BlocksSlider*>(last_hovered_slider_->getParentComponent()->getParentComponent());
  for (auto& modulation : current_connections_) {
    bool same_source = modulation->source->id == modulator_component->model_id_;
    bool same_target = modulation->target->id == slider->module_id_;
    bool same_parameter = modulation->parameter_name_ == slider->parameter_name_;
    if (same_source && same_target && same_parameter) {
      return;
    }
  }
  if (slider && slider->modulatable) {
    is_currently_modulating_slider_ = true;
    listener_->hovered(slider, modulator_component);
  }
}

void ModulationStateManager::activate(const std::vector<std::shared_ptr<model::Connection>>& connections) {
  current_connections_ = connections;
}

void ModulationStateManager::deactivate() {
  if (last_hovered_slider_) {
    if (auto box_slider = dynamic_cast<BlocksSlider*>(last_hovered_slider_->getParentComponent()->getParentComponent())) {
      if (box_slider->modulatable) {
        box_slider->stopModulationSelectionAnimation();
        box_slider->setIndicationHighlight(false, {});
      }
    }
  }
  last_hovered_slider_ = nullptr;
  is_currently_modulating_slider_ = false;
}