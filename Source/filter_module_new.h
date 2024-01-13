#include "module_new.h"
#include "vital/synthesis/filters/synth_filter.h"
#include "vital/synth_strings.h"

namespace model {
class FilterModule: public Block {
public:
  FilterModule(int number): Block("filter", number) {
    add({ .name = "model", .max = 7.0, .string_lookup = strings::kFilterModelNames, .value_scale = ValueScale::kIndexed });
    add({ .name = "style", .max = 4.0, .string_lookup = strings::kFilterStyleNames, .value_scale = ValueScale::kIndexed });
    add({ .name = "cutoff", .min = 8.0, .max = 136.0, .default_value = 30.0, .post_offset = -60.0 });
    add({ .name = "resonance", .min = 0.0, .max = 1.0, .default_value = 0.5, .display_units = "%", .display_name = "Q", .display_multiply = 100.0 });
    add({ .name = "blend", .max = 2.0 });
    add({ .name = "mix", .max = 1.0 });
  }
};
}