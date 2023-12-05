// double getAdjustedValue(double value, vital::ValueDetails details) { // this should be a static method in ValueDetails?? 
//   double adjusted_value = value;
//   switch (details.value_scale) {
//   case vital::ValueDetails::kQuadratic:
//     adjusted_value *= adjusted_value;
//     break;
//   case vital::ValueDetails::kCubic:
//     adjusted_value *= adjusted_value * adjusted_value;
//     break;
//   case vital::ValueDetails::kQuartic:
//     adjusted_value *= adjusted_value;
//     adjusted_value *= adjusted_value;
//     break;
//   case vital::ValueDetails::kExponential:
//     adjusted_value = powf(2.0, adjusted_value); //display_exponential_base_ will be different for each module. i think just env?
//     break;
//   case vital::ValueDetails::kSquareRoot:
//     adjusted_value = sqrtf(std::max(adjusted_value, 0.0));
//     break;
//   default:
//     break;
//   }
  
//   float display_multiply_ = 1.0f;

//   adjusted_value += details.post_offset;
//   if (details.display_invert)
//     adjusted_value = 1.0 / adjusted_value;
//   if (display_multiply_)
//     adjusted_value *= display_multiply_;
//   else
//     adjusted_value *= details.display_multiply;

//   return adjusted_value;
// }

namespace vital {
struct ValueDetails {
  enum ValueScale {
    kIndexed,
    kLinear,
    kQuadratic,
    kCubic,
    kQuartic,
    kSquareRoot,
    kExponential
  };

  std::string name;
  int version_added = 0;
  mono_float min = 0.0f;
  mono_float max = 1.0f;
  mono_float default_value = 0.0f;
  // post_offset used to offset quadratic and exponential scaling.
  mono_float post_offset = 0.0f;
  mono_float display_multiply = 1.0f;
  ValueScale value_scale = kLinear;
  bool display_invert = false;
  std::string display_units = "";
  std::string display_name;
  const std::string* string_lookup = nullptr;
  std::string local_description;
  bool audio_rate = false;
  bool smooth_value = false;
  bool internal_modulation = false;
  bool reset = false;
  Value* val;
  int decimal_places = 2;
  bool hidden = false;
  std::vector<std::string> choices; // equals to string_lookup basically... not the best design choice
};
}