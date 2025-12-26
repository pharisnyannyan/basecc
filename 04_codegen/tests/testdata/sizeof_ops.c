struct Pair { int left; char right; };

int global_value = 0;
int *global_ptr = &global_value;

int size_int() {
    return sizeof(int);
}

int size_char() {
    return sizeof(char);
}

int size_short() {
    return sizeof(short);
}

int size_pointer() {
    return sizeof(int *);
}

int size_global() {
    return sizeof(global_value);
}

int size_local() {
    int local;
    return sizeof(local);
}

int size_struct_type() {
    return sizeof(struct Pair);
}

int size_struct_value() {
    struct Pair pair;
    return sizeof(pair);
}

int size_deref() {
    return sizeof(*global_ptr);
}
