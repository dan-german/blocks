#pragma once
#include "vital/common/synth_parameters.h"

struct UIUtils {
  static double getAdjustedValue(double value, vital::ValueDetails details) {
    double adjusted_value = value;
    switch (details.value_scale) {
    case vital::ValueDetails::kQuadratic:
      adjusted_value *= adjusted_value;
      break;
    case vital::ValueDetails::kCubic:
      adjusted_value *= adjusted_value * adjusted_value;
      break;
    case vital::ValueDetails::kQuartic:
      adjusted_value *= adjusted_value;
      adjusted_value *= adjusted_value;
      break;
    case vital::ValueDetails::kExponential:
      adjusted_value = powf(2.0, adjusted_value);
      break;
    case vital::ValueDetails::kSquareRoot:
      adjusted_value = sqrtf(std::max(adjusted_value, 0.0));
      break;
    default:
      break;
    }

    float display_multiply_ = details.display_multiply;

    adjusted_value += details.post_offset;
    if (details.display_invert)
      adjusted_value = 1.0 / adjusted_value;
    if (display_multiply_)
      adjusted_value *= display_multiply_;
    else
      adjusted_value *= details.display_multiply;

    return adjusted_value;
  }

  static juce::String formatValue(float value, vital::ValueDetails details) {
    int max_decimal_places = details.decimal_places;
    juce::String format;
    if (details.value_scale == vital::ValueDetails::kIndexed)
      format = juce::String(value);
    else {
      if (max_decimal_places == 0)
        format = juce::String(std::roundf(value));
      else
        format = juce::String(value, 3);

      int display_characters = 5;
      if (format[0] == '-')
        display_characters += 1;

      format = format.substring(0, display_characters);
      if (format.getLastCharacter() == '.')
        format = format.removeCharacters(".");
    }

    // if (use_suffix_)
    //   return format + getDisplayDetails()->display_units;

    return format;
  }

  static juce::String getSliderTextFromValue(double value, vital::ValueDetails details) {
    if (details.string_lookup) {
      int lookup = vital::utils::iclamp(value, 0, details.max);
      return details.string_lookup[lookup];
    }

    double adjusted_value = getAdjustedValue(value, details);
    return formatValue(adjusted_value, details);
  }
};