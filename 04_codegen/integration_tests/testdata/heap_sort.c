int is_leq(int left, int right)
{
    int l = left;
    int r = right;

    while (l && r) {
        l = l - 1;
        r = r - 1;
    }

    if (l) {
        return 0;
    }

    return 1;
}

int swap_values(int *left, int *right)
{
    int temp = *left;
    *left = *right;
    *right = temp;
    return 0;
}

int sift_down(int *values, int start, int end)
{
    int root = start;
    int ignored = 0;

    while (1) {
        int child = root * 2 + 1;
        int swap_index = root;

        if (!is_leq(child, end)) {
            return 0;
        }

        if (!is_leq(*(values + child), *(values + swap_index))) {
            swap_index = child;
        }

        if (is_leq(child + 1, end)) {
            if (!is_leq(*(values + child + 1), *(values + swap_index))) {
                swap_index = child + 1;
            }
        }

        if (!(swap_index - root)) {
            return 0;
        }

        ignored = swap_values(values + root, values + swap_index);
        root = swap_index;
    }
}

int heap_sort(int *values, int count)
{
    int ignored = 0;

    if (!count) {
        return 0;
    }

    for (int start = (count - 2) / 2; start + 1; start = start - 1) {
        ignored = sift_down(values, start, count - 1);
    }

    for (int end = count - 1; end; end = end - 1) {
        ignored = swap_values(values, values + end);
        ignored = sift_down(values, 0, end - 1);
    }

    return 0;
}
