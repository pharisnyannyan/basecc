int loop_control()
{
    int count = 0;

    while (1) {
        count = count + 1;
        break;
    }

    for (int i = 2; i; i = i - 1) {
        if (i) {
            continue;
        }
        count = count + 10;
    }

    return count;
}
