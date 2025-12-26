typedef const int *ConstIntPtr;
typedef void *VoidPtr;

int value = 5;

int main() {
  VoidPtr vp = (void *)&value;
  ConstIntPtr cp = (ConstIntPtr)vp;
  return *(int *)cp;
}
