#pragma once
struct ID {
  std::string type;
  int number;
  std::string getName() { return type + ' ' + std::to_string(number); }
  inline bool operator==(const ID& other) const { return type == other.type && number == other.number; }
};