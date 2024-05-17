#include "processor_pool.h"
#include "vital/synthesis/modules/reverb_module.h"
#include "vital/synthesis/modules/delay_module.h"
#include "vital/synthesis/modules/distortion_module.h"
#include "vital/synthesis/modules/chorus_module.h"
#include "vital/synthesis/modules/flanger_module.h"
#include "vital/synthesis/modules/phaser_module.h"
#include "vital/synthesis/modules/sample_module.h"
#include "vital/synthesis/modules/filter_module.h"
#include "vital/synthesis/modules/lfo_module.h"
#include "vital/synthesis/modules/distortion_module.h"  

void ProcessorPool::spawn(SetupInput& input) {
  std::string all[] = { "osc", "reverb", "drive", "chorus", "flanger", "noise", "phaser", "delay" };
  for (auto code : all) {
    spawnProcessors(code, input);
  }
}

void ProcessorPool::spawnProcessors(std::string code, ProcessorPool::SetupInput& input) {
  for (int i = 0; i < model::MAX_MODULES_PER_TYPE; i++) {
    auto processor = processorFromCode(code, input);
    input.parent->addSubmodule(processor);
    input.parent->addProcessor(processor);
    map_[code].push_back(processor);
  }
}

vital::SynthModule* ProcessorPool::processorFromCode(std::string code, SetupInput& input) {
  vital::SynthModule* processor = nullptr;
  if (code == "osc") {
    auto osc = new vital::OscillatorModule();
    oscillators_.push_back(osc);
    processor = osc;
    processor->plug(input.reset, vital::OscillatorModule::kReset);
    processor->plug(input.retrigger, vital::OscillatorModule::kRetrigger);
    processor->plug(input.bent_midi, vital::OscillatorModule::kMidi);
    processor->plug(input.active_mask, vital::OscillatorModule::kActiveVoices);
  } else if (code == "filter") {
    processor = new vital::FilterModule(code);
    processor->plug(input.reset, vital::FilterModule::kReset);
    processor->plug(input.bent_midi, vital::FilterModule::kMidi);
    processor->plug(input.note_from_ref, vital::FilterModule::kKeytrack);
  } else if (code == "chorus") {
    processor = new vital::ChorusModule(input.beats_per_second);
    processor->plug(input.reset, vital::ChorusModule::kReset);
  } else if (code == "flanger") {
    processor = new vital::FlangerModule(input.beats_per_second);
  } else if (code == "phaser") {
    processor = new vital::PhaserModule(input.beats_per_second);
  } else if (code == "noise") {
    processor = new vital::SampleModule();
    processor->plug(input.reset, vital::SampleModule::kReset);
    processor->plug(input.note_count, vital::SampleModule::kNoteCount);
    processor->plug(input.bent_midi, vital::SampleModule::kMidi);
  } else if (code == "delay") {
    processor = new vital::DelayModule(input.beats_per_second);
    processor->plug(input.reset, vital::DelayModule::kReset);
  } else if (code == "reverb") {
    processor = new vital::ReverbModule();
    processor->plug(input.reset, vital::ReverbModule::kReset);
    processor->enable(false);
  } else if (code == "lfo") {
  } else if (code == "adsr") {
  } else if (code == "drive") {
    processor = new vital::DistortionModule();
    processor->plug(input.reset, vital::DistortionModule::kReset);
  } else {
  }
  return processor;
}

void ProcessorPool::enableAll(bool enable) {
  for (auto& pair : map_) {
    for (auto processor : pair.second) {
      processor->enable(enable);
      if (processor->control_map_.count("on"))
        processor->control_map_["on"]->set(0.0f);
    }
  }
}

vital::SynthModule* ProcessorPool::fetch(std::string& code) {
  auto& processors = map_[code];
  if (processors.size() == 0) {
    return nullptr;
  }
  auto processor = processors.back();
  processors.pop_back();
  processor->enable(true);
  return processor;
}

void ProcessorPool::retire(vital::SynthModule* processor, std::shared_ptr<model::Block> block) {
  processor->enable(false);
  map_[block->id.type].push_back(processor);
  if (processor->control_map_.count("on"))
    processor->control_map_["on"]->set(0.0f);
}

void ProcessorPool::load(std::unordered_map<std::string, int> count) {
  //  for (int i = 0; i < processor_count_map_[code]; i++) {
  //   auto processor = processorFromCode(code, input);
  //   input.parent->addSubmodule(processor);
  //   input.parent->addProcessor(processor);
  //   map_[code].push_back(processor);
  // } 
}