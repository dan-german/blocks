#pragma once
#include "module_new.h"
#include "vital/synthesis/effects/distortion.h"

namespace model {
class DriveModule: public Block {
public:
  DriveModule(int number): Block("drive", number) {
    add({ .name = "type", .max = 5.0f, .value_scale = ValueScale::kIndexed, .string_lookup = strings::kDistortionTypeNames });
    add({ .name = "drive", .min = vital::Distortion::kMinDrive, .max = vital::Distortion::kMaxDrive });
    add({ .name = "mix", .default_value = 1.0f });
  };
};
}