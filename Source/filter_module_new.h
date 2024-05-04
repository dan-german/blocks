#include "module_new.h"
#include "vital/synthesis/filters/synth_filter.h"
#include "vital/synth_strings.h"

namespace model {
class FilterModule: public Block {
public:
  FilterModule(int number): Block("filter", number) {
    add({ .name = "model", .max = 7.0, .value_scale = ValueScale::kIndexed , .string_lookup = strings::kFilterModelNames, .modulatable = false });
    add({ .name = "style", .max = 4.0, .value_scale = ValueScale::kIndexed , .string_lookup = strings::kFilterStyleNames, .modulatable = false });
    add({ .name = "cutoff", .min = 8.0, .max = 136.0, .default_value = 30.0, .post_offset = -60.0 });
    add({ .name = "resonance", .max = 1.0, .default_value = 0.5, .display_multiply = 100.0, .display_units = "%", .display_name = "Q" });
    add({ .name = "blend", .max = 2.0 });
    add({ .name = "mix" });
  }
};
}


// std::string name;
// int version_added = 0;
// mono_float min = 0.0f;
// mono_float max = 1.0f;
// mono_float default_value = 0.0f;
// // post_offset used to offset quadratic and exponential scaling.
// mono_float post_offset = 0.0f;
// mono_float display_multiply = 1.0f;
// ValueScale value_scale = kLinear;
// bool display_invert = false;
// std::string display_units = "";
// std::string display_name;
// const std::string* string_lookup = nullptr;
// std::string local_description;
// bool audio_rate = false;
// bool smooth_value = false;
// bool internal_modulation = false;
// bool reset = false;
// Value* value_processor;
// mono_float value;
// int decimal_places = 2;
// bool hidden = false;