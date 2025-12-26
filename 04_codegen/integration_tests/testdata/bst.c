struct Node {
    int value;
    struct Node *left;
    struct Node *right;
};

struct Node node_a;
struct Node node_b;
struct Node node_c;
struct Node node_d;
struct Node node_e;
struct Node node_f;
struct Node node_g;

struct Node *bst_find(struct Node *root, int value)
{
    struct Node *found = 0;

    if (!root) {
        return 0;
    }

    if (!(root->value - value)) {
        return root;
    }

    found = bst_find(root->left, value);
    if (found) {
        return found;
    }

    return bst_find(root->right, value);
}

struct Node *bst_build()
{
    struct Node *root = 0;

    node_a.value = 5;
    node_b.value = 3;
    node_c.value = 8;
    node_d.value = 1;
    node_e.value = 4;
    node_f.value = 7;
    node_g.value = 9;

    node_a.left = 0;
    node_a.right = 0;
    node_b.left = 0;
    node_b.right = 0;
    node_c.left = 0;
    node_c.right = 0;
    node_d.left = 0;
    node_d.right = 0;
    node_e.left = 0;
    node_e.right = 0;
    node_f.left = 0;
    node_f.right = 0;
    node_g.left = 0;
    node_g.right = 0;

    root = &node_a;
    node_a.left = &node_b;
    node_a.right = &node_c;
    node_b.left = &node_d;
    node_b.right = &node_e;
    node_c.left = &node_f;
    node_c.right = &node_g;

    return root;
}

int bst_search_sum()
{
    struct Node *root = bst_build();
    int total = 0;

    if (bst_find(root, 4)) {
        total = total + 4;
    }

    if (bst_find(root, 6)) {
        total = total + 100;
    }

    if (bst_find(root, 7)) {
        total = total + 7;
    }

    if (bst_find(root, 9)) {
        total = total + 9;
    }

    return total;
}
