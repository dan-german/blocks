#include "settings/UserSettings.h"

namespace {
std::unique_ptr<PropertiesFile> get_file();
}

UserSettings* UserSettings::shared() {
  static UserSettings instance;
  return &instance;
}

void UserSettings::set(StringRef k, const var& v) {
  file->setValue(k, v);
  file->saveIfNeeded();
}

int UserSettings::getInt(StringRef keyName, int defaultValue) {
  return file->getIntValue(keyName, defaultValue);
}

String UserSettings::getString(StringRef keyName, String& defaultValue) {
  return file->getValue(keyName, defaultValue);
}

UserSettings::UserSettings() {
  file = get_file();
}

namespace {
std::unique_ptr<PropertiesFile> get_file() {
  PropertiesFile::Options options;

  options.applicationName = "blocks";
  options.filenameSuffix = ".settings";
  options.folderName = "blocks";
  options.osxLibrarySubFolder = "Application Support";
  options.storageFormat = PropertiesFile::StorageFormat::storeAsXML;

  return std::make_unique<PropertiesFile>(options);
}
}
