struct Node {
  int value;
  struct Node *next;
};

struct Node node_a;
struct Node node_b;
struct Node node_c;
struct Node node_d;
struct Node node_e;

struct Node *list_init() {
  struct Node *head = &node_a;
  struct Node *temp = &node_a;

  temp->value = 1;
  temp->next = &node_b;

  temp = &node_b;
  temp->value = 2;
  temp->next = &node_c;

  temp = &node_c;
  temp->value = 3;
  temp->next = 0;

  temp = &node_d;
  temp->value = 4;
  temp->next = 0;

  temp = &node_e;
  temp->value = 5;
  temp->next = 0;

  return head;
}

struct Node *list_push_front(struct Node *head, struct Node *node) {
  node->next = head;
  return node;
}

struct Node *list_insert_after(struct Node *pos, struct Node *node) {
  node->next = pos->next;
  pos->next = node;
  return node;
}

struct Node *list_remove_after(struct Node *pos) {
  struct Node *removed = pos->next;

  if (!removed) {
    return 0;
  }

  pos->next = removed->next;
  removed->next = 0;
  return removed;
}

struct Node *list_find(struct Node *head, int value) {
  struct Node *cur = head;

  while (cur) {
    int diff = cur->value - value;
    if (!diff) {
      return cur;
    }
    cur = cur->next;
  }

  return 0;
}

int list_smoke() {
  struct Node *head = list_init();
  struct Node *inserted = 0;
  struct Node *removed = 0;
  struct Node *found = 0;
  int total = 0;

  head = list_push_front(head, &node_e);
  inserted = list_insert_after(head, &node_d);
  removed = list_remove_after(head);
  found = list_find(head, 2);

  total = total + head->value;
  if (removed) {
    total = total + removed->value;
  }
  if (found) {
    total = total + found->value;
  }

  return total;
}
