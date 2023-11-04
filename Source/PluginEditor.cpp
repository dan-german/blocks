/*
  ==============================================================================

  This file was auto-generated!

  It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "blocks_plugin_processor.h"
#include "PluginEditor.h"

void PluginEditor::paint(Graphics& g) { g.fillAll(Colour(28, 28, 28)); }
void PluginEditor::resized() { mainComponent.setBounds(getLocalBounds()); }
void PluginEditor::timerCallback() { }

PluginEditor::PluginEditor(juce::MidiKeyboardState& keyboard_state, PluginProcessor& p): AudioProcessorEditor(&p), processor(p), mainComponent(keyboard_state, &p) {
  setSize(1200, 770);

  setResizable(false, false);
  addAndMakeVisible(mainComponent);
}
