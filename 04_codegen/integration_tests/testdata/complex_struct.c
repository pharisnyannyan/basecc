struct Data {
  int a;
  int b;
  int c;
};

struct Data g_data;

int sum_data() {
  g_data.a = 10;
  g_data.b = 20;
  g_data.c = 30;
  return g_data.a + g_data.b + g_data.c;
}

struct Node {
  int value;
  struct Node *next;
};

int get_second_value(struct Node *head) {
  struct Node *second = head->next;
  return second->value;
}
