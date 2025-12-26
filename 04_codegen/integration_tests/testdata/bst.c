struct Node {
    int value;
    struct Node *left;
    struct Node *right;
};

extern void *malloc(unsigned long size);
extern void free(void *ptr);
extern long write(int fd, const void *buf, unsigned long count);

static void write_text(const char *text, unsigned long length)
{
    write(1, text, length);
}

static struct Node *bst_insert(struct Node *root, int value)
{
    struct Node *node = 0;

    if (!root) {
        node = malloc(sizeof(struct Node));
        if (!node) {
            return 0;
        }
        node->value = value;
        node->left = 0;
        node->right = 0;
        return node;
    }

    if (value < root->value) {
        root->left = bst_insert(root->left, value);
    } else if (value > root->value) {
        root->right = bst_insert(root->right, value);
    }

    return root;
}

static void bst_free(struct Node *root)
{
    if (!root) {
        return;
    }

    bst_free(root->left);
    bst_free(root->right);
    free(root);
}

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

    root = bst_insert(root, 5);
    root = bst_insert(root, 3);
    root = bst_insert(root, 8);
    root = bst_insert(root, 1);
    root = bst_insert(root, 4);
    root = bst_insert(root, 7);
    root = bst_insert(root, 9);

    return root;
}

int bst_search_sum()
{
    struct Node *root = bst_build();

    if (bst_find(root, 4)) {
        write_text("found 4\n", 8);
    } else {
        write_text("missing 4\n", 10);
    }

    if (bst_find(root, 6)) {
        write_text("found 6\n", 8);
    } else {
        write_text("missing 6\n", 10);
    }

    if (bst_find(root, 7)) {
        write_text("found 7\n", 8);
    } else {
        write_text("missing 7\n", 10);
    }

    if (bst_find(root, 9)) {
        write_text("found 9\n", 8);
    } else {
        write_text("missing 9\n", 10);
    }

    bst_free(root);
    return 0;
}
