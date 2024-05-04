#pragma once
#include <juce_data_structures/juce_data_structures.h>
using namespace juce;

class UserSettings {
public:
  static UserSettings* shared();

  void set(StringRef k, const var& v);
  int getInt(StringRef keyName, int defaultValue);
  String getString(StringRef keyName, String& defaultValue);
  const std::string last_connections_side_menu_state = "last_connections_side_menu_state";

private:
  UserSettings();
  std::unique_ptr<PropertiesFile> file;
};