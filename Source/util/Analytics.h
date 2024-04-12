#pragma once

#include "util/json.hpp"
#include <juce_core/juce_core.h>
#include <optional>

using namespace juce;
using json = nlohmann::json;

class Analytics {
private:
  String sessionID = juce::String::toHexString(juce::Random::getSystemRandom().nextInt64());
  std::string projectToken;
  json baseProperties;
  inline static Analytics* instance;
public:
  Analytics();
  static Analytics* shared();
  void setProperty(const String& key, const String& value);
  void sendEvent(const String& eventName, std::optional<std::string> customSessionID = {}, std::optional<json> extraProperties = {});
  void countAction(const String& message);
  std::map<String, int> eventCounter;
  void handleQuit();
  void handleLaunch(String wrapperType);
  void initProfileIfNeeded();
  void sendPOST(const juce::String& urlString, const json& bodyData);
  std::string sendGET(const juce::String& urlString);
};
