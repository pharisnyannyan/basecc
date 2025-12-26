extern int read(int fd, char *buf, int count);
extern int write(int fd, char *buf, int count);

int extern_io()
{
    char buffer[5];
    char out_msg[7];
    char err_msg[7];
    int got;
    int wrote;

    out_msg[0] = 115;
    out_msg[1] = 116;
    out_msg[2] = 100;
    out_msg[3] = 111;
    out_msg[4] = 117;
    out_msg[5] = 116;
    out_msg[6] = 10;
    err_msg[0] = 115;
    err_msg[1] = 116;
    err_msg[2] = 100;
    err_msg[3] = 101;
    err_msg[4] = 114;
    err_msg[5] = 114;
    err_msg[6] = 10;
    got = read(0, buffer, 5);
    wrote = write(1, out_msg, 7);
    wrote = write(2, err_msg, 7);
    if (got) {
        wrote = write(1, buffer, got);
    }
    return got + wrote;
}
