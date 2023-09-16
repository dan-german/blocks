/*
  ==============================================================================

    NoteHelper.h
    Created: 24 Jun 2021 10:45:54pm
    Author:  Dan German

  ==============================================================================
*/

struct NoteHelper {
  static const inline StringArray durationStrings = {
    "8",
    "4",
    "2",
    "1",
    "1/2",
    "1/4",
    "1/8",
    "1/16",
    "1/32",
    "1/64"
  };

  enum Duration {
    eightBars,
    fourBars,
    twoBars,
    oneBar,
    half,
    quarter,
    eighth,
    sixteenth,
    thirtyTwo,
    sixtyFour
  };

  static inline float indexToHertz(Duration duration, double bpm) {
    float bar = 240.0f;
    float hertz = -1.0f;

    switch (duration) {
    case eightBars: hertz = bar * 8; break;
    case fourBars: hertz = bar * 4; break;
    case twoBars: hertz = bar * 2; break;
    case oneBar: hertz = bar; break;
    case half: hertz = bar / 2; break;
    case quarter: hertz = bar / 4; break;
    case eighth: hertz = bar / 8; break;
    case sixteenth: hertz = bar / 16; break;
    case thirtyTwo: hertz = bar / 32; break;
    case sixtyFour: hertz = bar / 64; break;
    }

    return static_cast<float>(bpm / hertz);
  }
};
