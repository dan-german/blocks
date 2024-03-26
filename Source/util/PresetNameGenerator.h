#include <string>
#include <vector>
#include <cstdlib>

struct PresetNameGenerator {
  inline static const std::vector<std::string> adjectives = {
    "sparky", "glazed", "frosty", "breezy", "jazzy",
    "sleek", "gusty", "witty", "dapper", "lively",
    "serenity", "calm", "twilight", "haze", "harmony",
    "zen", "solitude", "horizon", "cloud", "ocean",
    "sunset", "silence", "moonlight", "thunder", "snowfall",
    "fog", "sunrise", "misty", "dreamy", "chirpy",
    "cozy", "crispy", "dewy", "gloomy", "lush",
    "mellow", "radiant", "shiny", "smoky", "velvety",
    "breezy", "rustling", "fragrant", "heavenly", "mystical",
    "peaceful", "resplendent", "tranquil", "sunny", "golden",
    "azure", "vibrant", "brilliant", "pulsing", "enchanting",
    "magical", "whimsical", "fantastical", "ethereal", "enigmatic",
    "serene", "blissful", "cozy", "delicate", "endless",
    "gentle", "graceful", "silken", "glowing", "blushing",
    "intense", "rhythmic", "mysterious", "euphoric", "dancing",
    "captivating", "alluring", "celestial", "dynamic", "ecstatic",
    "jubilant", "scenic", "tranquillizing", "ethereal", "spellbinding",
    "empowering", "exquisite", "fascinating", "soothing", "captivating",
    "illuminating", "intoxicating", "majestic", "mesmerizing", "invigorating",
    "enlivening", "enriching", "rejuvenating", "therapeutic", "restorative",
    "romantic", "refreshing", "blissful", "seraphic", "divine",
    "glorious", "heavenly", "idyllic", "pristine", "paradisiacal",
    "picturesque", "serendipitous", "sublime", "sumptuous", "ethereal",
    "atmospheric", "vintage", "cosmic", "bright", "illustrious",
    "tinkling", "static", "glimmering", "radiating", "shimmering", "twinkling",
    "fizzy", "sparkling", "aerated", "rascal", "lunar", "diamond", "silver"
  };

  inline static const std::vector<std::string> nouns = {
    "sky", "cloud", "ocean", "forest", "river",
    "mountain", "beach", "rain", "snow", "thunder",
    "leaf", "flower", "bird", "butterfly", "sunset",
    "moon", "sun", "star", "rainbow", "dew",
    "field", "meadow", "prairie", "desert", "canyon",
    "valley", "island", "tree", "breeze", "mist",
    "fog", "wave", "shore", "seashell", "rock",
    "pebble", "stream", "waterfall", "pond", "lake",
    "reflection", "glacier", "wildflower", "landscape", "creek",
    "thicket", "raindrop", "hurricane", "tornado", "cyclone",
    "blizzard", "avalanche", "hail", "drizzle", "downpour",
    "tide", "haze", "morning", "twilight", "daybreak",
    "dusk", "horizon", "bramble", "mushroom", "fern",
    "basin", "plateau", "cavern", "cliff", "coral",
    "islet", "marsh", "reef", "savanna", "volcano",
    "water", "bog", "cove", "creek", "asteroid", "gorge",
    "glade", "grotto", "lagoon", "rapids", "safari",
    "seabed", "slope", "steppe", "summit", "tidepool", "orbit",
    "acorn", "brook", "cactus", "clay", "creekbed",
    "delta", "desert", "dune", "earth", "frost",
    "glacier", "grain", "harbor", "haze", "icicle",
    "lakefront", "leaf", "lightning", "maple", "mesa",
    "moss", "oasis", "pebble", "riverbed", "saltwater",
    "seabreeze", "smoke", "spruce", "tide", "tundra",
    "galaxy", "nostalgia", "carbon", "oxygen",
    "matter", "comet", "ember", "meteor", "satellite"
  };

  static std::string generate() {
    auto adjIndex = std::rand() % PresetNameGenerator::adjectives.size();
    auto nounIndex = std::rand() % PresetNameGenerator::nouns.size();
    return adjectives[adjIndex] + " " + nouns[nounIndex];
  }
};