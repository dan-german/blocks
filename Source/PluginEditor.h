/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "blocks_plugin_processor.h"
#include "MainComponent.h"

class PluginEditor: public AudioProcessorEditor, juce::Timer {
private:
  PluginProcessor& processor;
  JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginEditor)
public:
  gui::MainComponent mainComponent;
  PluginEditor(juce::MidiKeyboardState& keyboard_state, PluginProcessor& processor);
  void paint(Graphics&) override;
  void resized() override;
  void timerCallback() override;
};
