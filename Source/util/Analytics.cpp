#include "util/Analytics.h"
#include "settings/UserSettings.h"
#include "BinaryData.h"
#include "generated/version_config.h"

Analytics::Analytics() {
  projectToken = String::fromUTF8(BinaryData::mixpanel_project_token_key, BinaryData::mixpanel_project_token_keySize).toStdString();

  baseProperties["time"] = Time::getCurrentTime().toMilliseconds() / 1000;
  baseProperties["OS"] = juce::SystemStats::getOperatingSystemName().toStdString();
  baseProperties["is64Bit"] = juce::SystemStats::isOperatingSystem64Bit();
  baseProperties["UserLanguage"] = juce::SystemStats::getUserLanguage().toStdString();
  baseProperties["UserRegion"] = juce::SystemStats::getUserRegion().toStdString();
  baseProperties["DisplayLanguage"] = juce::SystemStats::getDisplayLanguage().toStdString();
  baseProperties["DeviceDescription"] = juce::SystemStats::getDeviceDescription().toStdString();
  baseProperties["DeviceManufacturer"] = juce::SystemStats::getDeviceManufacturer().toStdString();
  baseProperties["NumLogicalCpus"] = juce::SystemStats::getNumCpus();
  baseProperties["NumPhysicalCpus"] = juce::SystemStats::getNumPhysicalCpus();
  baseProperties["CpuSpeedMHz"] = juce::SystemStats::getCpuSpeedInMegahertz();
  baseProperties["CpuVendor"] = juce::SystemStats::getCpuVendor().toStdString();
  baseProperties["CpuModel"] = juce::SystemStats::getCpuModel().toStdString();
  baseProperties["MemorySize"] = juce::SystemStats::getMemorySizeInMegabytes();
  baseProperties["session_id"] = sessionID.toStdString();
  baseProperties["distinct_id"] = juce::SystemStats::getUniqueDeviceID().toStdString();
  baseProperties["token"] = projectToken;
  baseProperties["ClientVersion"] = BLOCKS_VERSION
}

void Analytics::setProperty(const String& key, const String& value) {
  baseProperties[key.toStdString()] = value.toStdString();
}

void Analytics::countAction(const String& message) {
  eventCounter[message]++;
}

Analytics* Analytics::shared() {
  if (instance == nullptr) instance = new Analytics();
  return instance;
}

void Analytics::sendHTTPRequest(const juce::String& urlString, const json& bodyData) {
  auto options = juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress).withExtraHeaders("Content-Type: application/json");
  juce::URL(urlString).withPOSTData(bodyData.dump()).createInputStream(options);
}

void Analytics::handleQuit() {
  UserSettings::shared()->set("SessionEndTimestamp", Time::getCurrentTime().toMilliseconds() / 1000);
  UserSettings::shared()->set("session_id", sessionID);

  json eventCounterJSON;
  for (auto& [key, value] : eventCounter)
    eventCounterJSON[key.toStdString()] = value;

  UserSettings::shared()->set("ActionCounter", String(eventCounterJSON.dump()));
}

void Analytics::handleLaunch(String wrapperType) {
  setProperty("WrapperType", wrapperType);

  auto emptyString = juce::String("");
  json extraProps;

  auto eventCounter = UserSettings::shared()->getString("ActionCounter", emptyString);
  if (eventCounter != "") extraProps = json::parse(eventCounter.toStdString());

  auto sessionEndTimestamp = UserSettings::shared()->getInt("SessionEndTimestamp", 0);
  if (sessionEndTimestamp != 0) {
    auto sessionID = UserSettings::shared()->getString("session_id", emptyString);
    if (sessionID != "null") {
      extraProps["Duration"] = Time::getCurrentTime().toMilliseconds() / 1000 - sessionEndTimestamp;
      extraProps["time"] = sessionEndTimestamp;
      Analytics::shared()->sendEvent("Session End", sessionID.toStdString(), extraProps);
    }
  }

  Analytics::shared()->sendEvent("Session Start");
}

void Analytics::initProfileIfNeeded() {
  auto falseString = juce::String("false");
  auto initialized = UserSettings::shared()->getString("ProfileInitialized", falseString);
  if (initialized == "true") return;

  auto req = [this] {
    json body;
    body["$distinct_id"] = baseProperties["distinct_id"];
    body["$token"] = projectToken;

    auto now = Time::getCurrentTime();

    json props;
    props["First Launch Time Formatted"] = now.formatted("%Y-%m-%d %H:%M:%S").toStdString();
    props["First Launch Timestamp"] = now.toMilliseconds() / 1000;
    body["$set_once"] = props;

    json array;
    array.push_back(body);

    sendHTTPRequest("https://api.mixpanel.com/engage#profile-set-once", array);
    UserSettings::shared()->set("ProfileInitialized", juce::String("true"));
  };

  juce::Thread::launch(req);
}

void Analytics::sendEvent(const String& eventName, std::optional<std::string> customSessionID, std::optional<json> extraProperties) {
  auto req = [this, eventName, customSessionID, extraProperties] {
    json event;
    event["event"] = eventName.toStdString();
    event["properties"] = baseProperties;

    if (extraProperties.has_value())
      for (auto& [key, value] : extraProperties.value().items())
        event["properties"][key] = value;

    if (customSessionID.has_value()) {
      event["properties"]["session_id"] = customSessionID.value();
    }

    json array;
    array.push_back(event);

    sendHTTPRequest("https://api.mixpanel.com/track", array);
  };

  juce::Thread::launch(req);
}