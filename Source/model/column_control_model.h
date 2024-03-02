namespace model {
class ColumnControl: public Module {
public:
  ColumnControl(int number): Module("column control", number) {
    add({ .name = "level", .default_value = 1.0 });
    add({ .name = "pan", .min = -1.0 });
  }
};
}