int not_zero() {
    return !0;
}

int logical_and() {
    return 1 && 0;
}

int logical_or() {
    return 0 || 1;
}

int short_circuit_and_div0() {
    return 0 && (1 / 0);
}

int short_circuit_or_div0() {
    return 1 || (1 / 0);
}
