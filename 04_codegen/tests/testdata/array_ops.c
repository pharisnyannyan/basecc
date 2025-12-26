int global[3];

int main() {
    int local[2];
    global[0] = 1;
    global[1] = 2;
    local[0] = global[0] + global[1];
    local[1] = 7;
    return local[0] + local[1];
}
