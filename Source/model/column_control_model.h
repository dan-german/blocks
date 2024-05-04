#pragma once
namespace model {
class ColumnControl: public Module {
public:
  ColumnControl(int number): Module("column control", number) {
    display_name = "lane " + std::to_string(number);
    add({ .name = "level", .default_value = 1.0 });
    add({ .name = "pan", .min = -1.0 });
  }
};
}