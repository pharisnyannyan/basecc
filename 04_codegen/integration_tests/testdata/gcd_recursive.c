int gcd_recursive(int a, int b)
{
    if (!b) {
        return a;
    }

    return gcd_recursive(b, a % b);
}
