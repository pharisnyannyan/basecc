#include <stdlib.h>

int extern_io(void);

int main(void) {
  int ignored;

  ignored = extern_io();
  if (ignored) {
    return 0;
  }
  return 0;
}
