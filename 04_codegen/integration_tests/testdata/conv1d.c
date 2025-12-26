int conv1d(int *a, int n, int *b, int m, int *out)
{
    int size = n + m - 1;

    for (int index = 0; size - index; index = index + 1) {
        out[index] = 0;
    }

    for (int row = 0; n - row; row = row + 1) {
        for (int col = 0; m - col; col = col + 1) {
            out[row + col] = out[row + col] + a[row] * b[col];
        }
    }

    return 0;
}
