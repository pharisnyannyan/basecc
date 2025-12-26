int global_value = 5;
static int static_value;
static const char static_flag = 1;
static short static_numbers[2];
static int *static_ptr = &global_value;

static int add(int left, int right) {
  return left + right;
}

int main() {
  static int local_total = 3;
  static char local_letter;
  static const short local_const = 2;
  static int *local_ptr = &global_value;
  static int local_array[2];

  int sum = add(local_total, local_const);
  local_total = sum;
  return local_total;
}
