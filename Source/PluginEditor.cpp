/*
  ==============================================================================

  This file was auto-generated!

  It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/
#include "PluginProcessor.h"
#include "PluginEditor.h"

void PluginEditor::paint(Graphics& g) { g.fillAll(Colour(28, 28, 28)); }
void PluginEditor::resized() { mainComponent.setBounds(getLocalBounds()); }
void PluginEditor::timerCallback() { }

PluginEditor::PluginEditor(juce::MidiKeyboardState& keyboard_state, PluginProcessor& p): AudioProcessorEditor(&p), processor(p), mainComponent(keyboard_state, &p.synth) {
  setSize(1200, 770);

  setResizable(false, false);
  addAndMakeVisible(mainComponent);
}
