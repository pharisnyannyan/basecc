int add() {
  return 3 + 4;
}

int sub() {
  return 10 - 2;
}

int mul() {
  return 6 * 7;
}

int divide() {
  return 20 / 4;
}

int mod() {
  return 20 % 6;
}

int mixed() {
  return (1 + 2) * 3 - 4 / 2 % 3;
}

int unary() {
  return -(1 + 2) + +3;
}

int nested_parens() {
  return ((1 + 2) * (3 - 4)) / (5 + 6);
}

int triple_nested() {
  return (((1 + 2) + 3) * (4 + (5 - 6)));
}
