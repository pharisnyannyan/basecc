enum Color { RED, GREEN, BLUE = 10, YELLOW };

enum Color g_color = GREEN;

int get_red() {
  return RED;
}
int get_blue() {
  return BLUE;
}
int get_yellow() {
  return YELLOW;
}

int test_enum_logic() {
  enum Color c = RED;
  c = YELLOW;
  return c;
}
