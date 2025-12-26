extern int write(int fd, const char *buf, int count);
extern void *malloc(int size);
extern int free(void *ptr);

int main() {
  char buffer[4];
  void *mem;
  int written;
  int released;
  written = write(1, buffer, 0);
  mem = malloc(4);
  released = free(mem);
  return written + released;
}
