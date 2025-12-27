CC ?= clang
LL_CC ?= $(CC)
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -I../include -I../../03_checker/include -I../../02_parser/include \
	-I../../01_lexer/include

BUILD_DIR := build
LL := ../build/codegen_arithmetic.ll
INPUT := testdata/arithmetic_runtime.c
EXPECTED := arithmetic_driver_expected.txt
FIB_LL := ../build/codegen_fibonacci.ll
FIB_INPUT := testdata/fibonacci.c
FIB_EXPECTED := fibonacci_driver_expected.txt
FOR_LL := ../build/codegen_for_loop.ll
FOR_INPUT := testdata/for_loop.c
FOR_EXPECTED := for_loop_driver_expected.txt
SWAP_LL := ../build/codegen_swap.ll
SWAP_INPUT := testdata/swap.c
SWAP_EXPECTED := swap_driver_expected.txt
DOUBLE_PTR_LL := ../build/codegen_double_pointer.ll
DOUBLE_PTR_INPUT := testdata/double_pointer.c
DOUBLE_PTR_EXPECTED := double_pointer_driver_expected.txt
FILL_LL := ../build/codegen_fill_array.ll
FILL_INPUT := testdata/fill_array.c
FILL_EXPECTED := fill_array_driver_expected.txt
QUICK_SORT_LL := ../build/codegen_quick_sort.ll
QUICK_SORT_INPUT := testdata/quick_sort.c
QUICK_SORT_EXPECTED := quick_sort_driver_expected.txt
MERGE_SORT_LL := ../build/codegen_merge_sort.ll
MERGE_SORT_INPUT := testdata/merge_sort.c
MERGE_SORT_EXPECTED := merge_sort_driver_expected.txt
HEAP_SORT_LL := ../build/codegen_heap_sort.ll
HEAP_SORT_INPUT := testdata/heap_sort.c
HEAP_SORT_EXPECTED := heap_sort_driver_expected.txt
REVERSE_STRING_LL := ../build/codegen_reverse_string.ll
REVERSE_STRING_INPUT := testdata/reverse_string.c
REVERSE_STRING_EXPECTED := reverse_string_driver_expected.txt
LOOP_CONTROL_LL := ../build/codegen_loop_control.ll
LOOP_CONTROL_INPUT := testdata/loop_control.c
LOOP_CONTROL_EXPECTED := loop_control_driver_expected.txt
PRIMES_LL := ../build/codegen_primes.ll
PRIMES_INPUT := testdata/primes.c
PRIMES_EXPECTED := primes_driver_expected.txt
BST_LL := ../build/codegen_bst.ll
BST_INPUT := testdata/bst.c
BST_EXPECTED := bst_driver_expected.txt
SIEVE_LL := ../build/codegen_sieve_primes.ll
SIEVE_INPUT := testdata/sieve_primes.c
SIEVE_EXPECTED := sieve_primes_driver_expected.txt
GCD_LL := ../build/codegen_gcd_recursive.ll
GCD_INPUT := testdata/gcd_recursive.c
GCD_EXPECTED := gcd_recursive_driver_expected.txt
CONV_LL := ../build/codegen_conv1d.ll
CONV_INPUT := testdata/conv1d.c
CONV_EXPECTED := conv1d_driver_expected.txt
STRUCT_LL := ../build/codegen_struct_basic.ll
STRUCT_INPUT := testdata/struct_basic.c
STRUCT_EXPECTED := struct_basic_driver_expected.txt
STRUCT_LIST_LL := ../build/codegen_struct_linked_list.ll
STRUCT_LIST_INPUT := testdata/struct_linked_list.c
STRUCT_LIST_EXPECTED := struct_linked_list_driver_expected.txt
EXTERN_LL := ../build/codegen_extern_calls.ll
EXTERN_INPUT := testdata/extern_calls.c
EXTERN_EXPECTED := extern_calls_driver_expected.txt
EXTERN_IO_LL := ../build/codegen_extern_io.ll
EXTERN_IO_INPUT := testdata/extern_io.c
EXTERN_IO_EXPECTED := extern_io_driver_expected.txt
EXTERN_IO_ERR_EXPECTED := extern_io_driver_expected_err.txt

ENUM_LL := $(BUILD_DIR)/codegen_enum.ll
ENUM_INPUT := testdata/enum_test.c
ENUM_EXPECTED := enum_driver_expected.txt

STATIC_LL := $(BUILD_DIR)/codegen_static_local.ll
STATIC_INPUT := testdata/static_local.c
STATIC_EXPECTED := static_local_driver_expected.txt

COMPLEX_LL := $(BUILD_DIR)/codegen_complex_struct.ll
COMPLEX_INPUT := testdata/complex_struct.c
COMPLEX_EXPECTED := complex_struct_driver_expected.txt

SIZEOF_LL := $(BUILD_DIR)/codegen_sizeof_test.ll
SIZEOF_INPUT := testdata/sizeof_test.c
SIZEOF_EXPECTED := sizeof_driver_expected.txt

CODEGEN_LIB := ../build/libcodegen.a
CHECKER_LIB := ../../03_checker/build/libchecker.a
PARSER_LIB := ../../02_parser/build/libparser.a
LEXER_LIB := ../../01_lexer/build/liblexer.a

CODEGEN_BIN := $(BUILD_DIR)/run_codegen
CODEGEN_SRC := run_codegen.c
OBJ := $(BUILD_DIR)/arithmetic.o
BIN := $(BUILD_DIR)/arithmetic_driver
DRIVER := arithmetic_driver.c
OUTPUT := $(BUILD_DIR)/arithmetic_output.txt
FIB_OBJ := $(BUILD_DIR)/fibonacci.o
FIB_BIN := $(BUILD_DIR)/fibonacci_driver
FIB_DRIVER := fibonacci_driver.c
FIB_OUTPUT := $(BUILD_DIR)/fibonacci_output.txt
FOR_OBJ := $(BUILD_DIR)/for_loop.o
FOR_BIN := $(BUILD_DIR)/for_loop_driver
FOR_DRIVER := for_loop_driver.c
FOR_OUTPUT := $(BUILD_DIR)/for_loop_output.txt
SWAP_OBJ := $(BUILD_DIR)/swap.o
SWAP_BIN := $(BUILD_DIR)/swap_driver
SWAP_DRIVER := swap_driver.c
SWAP_OUTPUT := $(BUILD_DIR)/swap_output.txt
DOUBLE_PTR_OBJ := $(BUILD_DIR)/double_pointer.o
DOUBLE_PTR_BIN := $(BUILD_DIR)/double_pointer_driver
DOUBLE_PTR_DRIVER := double_pointer_driver.c
DOUBLE_PTR_OUTPUT := $(BUILD_DIR)/double_pointer_output.txt
FILL_OBJ := $(BUILD_DIR)/fill_array.o
FILL_BIN := $(BUILD_DIR)/fill_array_driver
FILL_DRIVER := fill_array_driver.c
FILL_OUTPUT := $(BUILD_DIR)/fill_array_output.txt
QUICK_SORT_OBJ := $(BUILD_DIR)/quick_sort.o
QUICK_SORT_BIN := $(BUILD_DIR)/quick_sort_driver
QUICK_SORT_DRIVER := quick_sort_driver.c
QUICK_SORT_OUTPUT := $(BUILD_DIR)/quick_sort_output.txt
MERGE_SORT_OBJ := $(BUILD_DIR)/merge_sort.o
MERGE_SORT_BIN := $(BUILD_DIR)/merge_sort_driver
MERGE_SORT_DRIVER := merge_sort_driver.c
MERGE_SORT_OUTPUT := $(BUILD_DIR)/merge_sort_output.txt
HEAP_SORT_OBJ := $(BUILD_DIR)/heap_sort.o
HEAP_SORT_BIN := $(BUILD_DIR)/heap_sort_driver
HEAP_SORT_DRIVER := heap_sort_driver.c
HEAP_SORT_OUTPUT := $(BUILD_DIR)/heap_sort_output.txt
REVERSE_STRING_OBJ := $(BUILD_DIR)/reverse_string.o
REVERSE_STRING_BIN := $(BUILD_DIR)/reverse_string_driver
REVERSE_STRING_DRIVER := reverse_string_driver.c
REVERSE_STRING_OUTPUT := $(BUILD_DIR)/reverse_string_output.txt
LOOP_CONTROL_OBJ := $(BUILD_DIR)/loop_control.o
LOOP_CONTROL_BIN := $(BUILD_DIR)/loop_control_driver
LOOP_CONTROL_DRIVER := loop_control_driver.c
LOOP_CONTROL_OUTPUT := $(BUILD_DIR)/loop_control_output.txt
PRIMES_OBJ := $(BUILD_DIR)/primes.o
PRIMES_BIN := $(BUILD_DIR)/primes_driver
PRIMES_DRIVER := primes_driver.c
PRIMES_OUTPUT := $(BUILD_DIR)/primes_output.txt
BST_OBJ := $(BUILD_DIR)/bst.o
BST_BIN := $(BUILD_DIR)/bst_driver
BST_DRIVER := bst_driver.c
BST_OUTPUT := $(BUILD_DIR)/bst_output.txt
SIEVE_OBJ := $(BUILD_DIR)/sieve_primes.o
SIEVE_BIN := $(BUILD_DIR)/sieve_primes_driver
SIEVE_DRIVER := sieve_primes_driver.c
SIEVE_OUTPUT := $(BUILD_DIR)/sieve_primes_output.txt
GCD_OBJ := $(BUILD_DIR)/gcd_recursive.o
GCD_BIN := $(BUILD_DIR)/gcd_recursive_driver
GCD_DRIVER := gcd_recursive_driver.c
GCD_OUTPUT := $(BUILD_DIR)/gcd_recursive_output.txt
CONV_OBJ := $(BUILD_DIR)/conv1d.o
CONV_BIN := $(BUILD_DIR)/conv1d_driver
CONV_DRIVER := conv1d_driver.c
CONV_OUTPUT := $(BUILD_DIR)/conv1d_output.txt
STRUCT_OBJ := $(BUILD_DIR)/struct_basic.o
STRUCT_BIN := $(BUILD_DIR)/struct_basic_driver
STRUCT_DRIVER := struct_basic_driver.c
STRUCT_OUTPUT := $(BUILD_DIR)/struct_basic_output.txt
STRUCT_LIST_OBJ := $(BUILD_DIR)/struct_linked_list.o
STRUCT_LIST_BIN := $(BUILD_DIR)/struct_linked_list_driver
STRUCT_LIST_DRIVER := struct_linked_list_driver.c
STRUCT_LIST_OUTPUT := $(BUILD_DIR)/struct_linked_list_output.txt
EXTERN_OBJ := $(BUILD_DIR)/extern_calls.o
EXTERN_BIN := $(BUILD_DIR)/extern_calls_driver
EXTERN_DRIVER := extern_calls_driver.c
EXTERN_OUTPUT := $(BUILD_DIR)/extern_calls_output.txt
EXTERN_IO_OBJ := $(BUILD_DIR)/extern_io.o
EXTERN_IO_BIN := $(BUILD_DIR)/extern_io_driver
EXTERN_IO_DRIVER := extern_io_driver.c
EXTERN_IO_OUTPUT := $(BUILD_DIR)/extern_io_output.txt
EXTERN_IO_ERR_OUTPUT := $(BUILD_DIR)/extern_io_error.txt

ENUM_OBJ := $(BUILD_DIR)/enum.o
ENUM_BIN := $(BUILD_DIR)/enum_driver
ENUM_DRIVER := enum_driver.c
ENUM_OUTPUT := $(BUILD_DIR)/enum_output.txt

STATIC_OBJ := $(BUILD_DIR)/static_local.o
STATIC_BIN := $(BUILD_DIR)/static_local_driver
STATIC_DRIVER := static_local_driver.c
STATIC_OUTPUT := $(BUILD_DIR)/static_local_output.txt

COMPLEX_OBJ := $(BUILD_DIR)/complex_struct.o
COMPLEX_BIN := $(BUILD_DIR)/complex_struct_driver
COMPLEX_DRIVER := complex_struct_driver.c
COMPLEX_OUTPUT := $(BUILD_DIR)/complex_struct_output.txt

SIZEOF_OBJ := $(BUILD_DIR)/sizeof_test.o
SIZEOF_BIN := $(BUILD_DIR)/sizeof_driver
SIZEOF_DRIVER := sizeof_driver.c
SIZEOF_OUTPUT := $(BUILD_DIR)/sizeof_output.txt

.PHONY: all compile generate run verify clean

all: $(BIN) $(FIB_BIN) $(FOR_BIN) $(SWAP_BIN) $(DOUBLE_PTR_BIN) $(FILL_BIN) \
	$(QUICK_SORT_BIN) $(MERGE_SORT_BIN) $(HEAP_SORT_BIN) \
	$(REVERSE_STRING_BIN) $(LOOP_CONTROL_BIN) $(PRIMES_BIN) \
	$(BST_BIN) $(SIEVE_BIN) $(GCD_BIN) $(CONV_BIN) \
	$(STRUCT_BIN) $(STRUCT_LIST_BIN) $(EXTERN_BIN) $(EXTERN_IO_BIN) \
	$(ENUM_BIN) $(STATIC_BIN) $(COMPLEX_BIN) $(SIZEOF_BIN)

generate: $(LL) $(FIB_LL) $(FOR_LL) $(SWAP_LL) $(DOUBLE_PTR_LL) $(FILL_LL) \
	$(QUICK_SORT_LL) $(MERGE_SORT_LL) $(HEAP_SORT_LL) \
	$(REVERSE_STRING_LL) $(LOOP_CONTROL_LL) $(PRIMES_LL) \
	$(BST_LL) $(SIEVE_LL) $(GCD_LL) $(CONV_LL) \
	$(STRUCT_LL) $(STRUCT_LIST_LL) $(EXTERN_LL) $(EXTERN_IO_LL) \
	$(ENUM_LL) $(STATIC_LL) $(COMPLEX_LL) $(SIZEOF_LL)

$(LL): $(CODEGEN_BIN) $(INPUT)
	./$(CODEGEN_BIN) $(INPUT) $(LL)

$(FIB_LL): $(CODEGEN_BIN) $(FIB_INPUT)
	./$(CODEGEN_BIN) $(FIB_INPUT) $(FIB_LL)

$(FOR_LL): $(CODEGEN_BIN) $(FOR_INPUT)
	./$(CODEGEN_BIN) $(FOR_INPUT) $(FOR_LL)

$(SWAP_LL): $(CODEGEN_BIN) $(SWAP_INPUT)
	./$(CODEGEN_BIN) $(SWAP_INPUT) $(SWAP_LL)

$(DOUBLE_PTR_LL): $(CODEGEN_BIN) $(DOUBLE_PTR_INPUT)
	./$(CODEGEN_BIN) $(DOUBLE_PTR_INPUT) $(DOUBLE_PTR_LL)

$(FILL_LL): $(CODEGEN_BIN) $(FILL_INPUT)
	./$(CODEGEN_BIN) $(FILL_INPUT) $(FILL_LL)

$(QUICK_SORT_LL): $(CODEGEN_BIN) $(QUICK_SORT_INPUT)
	./$(CODEGEN_BIN) $(QUICK_SORT_INPUT) $(QUICK_SORT_LL)

$(MERGE_SORT_LL): $(CODEGEN_BIN) $(MERGE_SORT_INPUT)
	./$(CODEGEN_BIN) $(MERGE_SORT_INPUT) $(MERGE_SORT_LL)

$(HEAP_SORT_LL): $(CODEGEN_BIN) $(HEAP_SORT_INPUT)
	./$(CODEGEN_BIN) $(HEAP_SORT_INPUT) $(HEAP_SORT_LL)

$(REVERSE_STRING_LL): $(CODEGEN_BIN) $(REVERSE_STRING_INPUT)
	./$(CODEGEN_BIN) $(REVERSE_STRING_INPUT) $(REVERSE_STRING_LL)

$(LOOP_CONTROL_LL): $(CODEGEN_BIN) $(LOOP_CONTROL_INPUT)
	./$(CODEGEN_BIN) $(LOOP_CONTROL_INPUT) $(LOOP_CONTROL_LL)

$(PRIMES_LL): $(CODEGEN_BIN) $(PRIMES_INPUT)
	./$(CODEGEN_BIN) $(PRIMES_INPUT) $(PRIMES_LL)

$(BST_LL): $(CODEGEN_BIN) $(BST_INPUT)
	./$(CODEGEN_BIN) $(BST_INPUT) $(BST_LL)

$(SIEVE_LL): $(CODEGEN_BIN) $(SIEVE_INPUT)
	./$(CODEGEN_BIN) $(SIEVE_INPUT) $(SIEVE_LL)

$(GCD_LL): $(CODEGEN_BIN) $(GCD_INPUT)
	./$(CODEGEN_BIN) $(GCD_INPUT) $(GCD_LL)

$(CONV_LL): $(CODEGEN_BIN) $(CONV_INPUT)
	./$(CODEGEN_BIN) $(CONV_INPUT) $(CONV_LL)

$(STRUCT_LL): $(CODEGEN_BIN) $(STRUCT_INPUT)
	./$(CODEGEN_BIN) $(STRUCT_INPUT) $(STRUCT_LL)

$(STRUCT_LIST_LL): $(CODEGEN_BIN) $(STRUCT_LIST_INPUT)
	./$(CODEGEN_BIN) $(STRUCT_LIST_INPUT) $(STRUCT_LIST_LL)

$(EXTERN_LL): $(CODEGEN_BIN) $(EXTERN_INPUT)
	./$(CODEGEN_BIN) $(EXTERN_INPUT) $(EXTERN_LL)

$(EXTERN_IO_LL): $(CODEGEN_BIN) $(EXTERN_IO_INPUT)
	./$(CODEGEN_BIN) $(EXTERN_IO_INPUT) $(EXTERN_IO_LL)

$(ENUM_LL): $(CODEGEN_BIN) $(ENUM_INPUT)
	./$(CODEGEN_BIN) $(ENUM_INPUT) $(ENUM_LL)

$(STATIC_LL): $(CODEGEN_BIN) $(STATIC_INPUT)
	./$(CODEGEN_BIN) $(STATIC_INPUT) $(STATIC_LL)

$(COMPLEX_LL): $(CODEGEN_BIN) $(COMPLEX_INPUT)
	./$(CODEGEN_BIN) $(COMPLEX_INPUT) $(COMPLEX_LL)

$(SIZEOF_LL): $(CODEGEN_BIN) $(SIZEOF_INPUT)
	./$(CODEGEN_BIN) $(SIZEOF_INPUT) $(SIZEOF_LL)

compile: generate $(OBJ) $(FIB_OBJ) $(FOR_OBJ) $(SWAP_OBJ) $(DOUBLE_PTR_OBJ) \
	$(FILL_OBJ) \
	$(QUICK_SORT_OBJ) $(MERGE_SORT_OBJ) $(HEAP_SORT_OBJ) \
	$(REVERSE_STRING_OBJ) $(LOOP_CONTROL_OBJ) $(PRIMES_OBJ) \
	$(BST_OBJ) $(SIEVE_OBJ) $(GCD_OBJ) $(CONV_OBJ) \
	$(STRUCT_OBJ) $(STRUCT_LIST_OBJ) $(EXTERN_OBJ) $(EXTERN_IO_OBJ) \
	$(ENUM_OBJ) $(STATIC_OBJ) $(COMPLEX_OBJ) $(SIZEOF_OBJ)

run: all $(OUTPUT) $(FIB_OUTPUT) $(FOR_OUTPUT) $(SWAP_OUTPUT) \
	$(DOUBLE_PTR_OUTPUT) \
	$(FILL_OUTPUT) $(QUICK_SORT_OUTPUT) $(MERGE_SORT_OUTPUT) \
	$(HEAP_SORT_OUTPUT) $(REVERSE_STRING_OUTPUT) $(LOOP_CONTROL_OUTPUT) \
	$(PRIMES_OUTPUT) $(BST_OUTPUT) $(SIEVE_OUTPUT) $(GCD_OUTPUT) \
	$(CONV_OUTPUT) $(STRUCT_OUTPUT) $(STRUCT_LIST_OUTPUT) $(EXTERN_OUTPUT) \
	$(EXTERN_IO_OUTPUT) $(ENUM_OUTPUT) $(STATIC_OUTPUT) $(COMPLEX_OUTPUT) \
	$(SIZEOF_OUTPUT)

verify: run
	cmp -s $(OUTPUT) $(EXPECTED)
	cmp -s $(FIB_OUTPUT) $(FIB_EXPECTED)
	cmp -s $(FOR_OUTPUT) $(FOR_EXPECTED)
	cmp -s $(SWAP_OUTPUT) $(SWAP_EXPECTED)
	cmp -s $(DOUBLE_PTR_OUTPUT) $(DOUBLE_PTR_EXPECTED)
	cmp -s $(FILL_OUTPUT) $(FILL_EXPECTED)
	cmp -s $(QUICK_SORT_OUTPUT) $(QUICK_SORT_EXPECTED)
	cmp -s $(MERGE_SORT_OUTPUT) $(MERGE_SORT_EXPECTED)
	cmp -s $(HEAP_SORT_OUTPUT) $(HEAP_SORT_EXPECTED)
	cmp -s $(REVERSE_STRING_OUTPUT) $(REVERSE_STRING_EXPECTED)
	cmp -s $(LOOP_CONTROL_OUTPUT) $(LOOP_CONTROL_EXPECTED)
	cmp -s $(PRIMES_OUTPUT) $(PRIMES_EXPECTED)
	cmp -s $(BST_OUTPUT) $(BST_EXPECTED)
	cmp -s $(SIEVE_OUTPUT) $(SIEVE_EXPECTED)
	cmp -s $(GCD_OUTPUT) $(GCD_EXPECTED)
	cmp -s $(CONV_OUTPUT) $(CONV_EXPECTED)
	cmp -s $(STRUCT_OUTPUT) $(STRUCT_EXPECTED)
	cmp -s $(STRUCT_LIST_OUTPUT) $(STRUCT_LIST_EXPECTED)
	cmp -s $(EXTERN_OUTPUT) $(EXTERN_EXPECTED)
	cmp -s $(EXTERN_IO_OUTPUT) $(EXTERN_IO_EXPECTED)
	cmp -s $(ENUM_OUTPUT) $(ENUM_EXPECTED)
	cmp -s $(STATIC_OUTPUT) $(STATIC_EXPECTED)
	cmp -s $(COMPLEX_OUTPUT) $(COMPLEX_EXPECTED)
	cmp -s $(SIZEOF_OUTPUT) $(SIZEOF_EXPECTED)
	cmp -s $(EXTERN_IO_ERR_OUTPUT) $(EXTERN_IO_ERR_EXPECTED)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CODEGEN_BIN): $(CODEGEN_SRC) $(CODEGEN_LIB) $(CHECKER_LIB) \
	$(PARSER_LIB) $(LEXER_LIB) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(CODEGEN_SRC) $(CODEGEN_LIB) \
		$(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)

$(OBJ): $(LL) | $(BUILD_DIR)
	$(LL_CC) -c $(LL) -o $(OBJ)

$(FIB_OBJ): $(FIB_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(FIB_LL) -o $(FIB_OBJ)

$(FOR_OBJ): $(FOR_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(FOR_LL) -o $(FOR_OBJ)

$(SWAP_OBJ): $(SWAP_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(SWAP_LL) -o $(SWAP_OBJ)

$(DOUBLE_PTR_OBJ): $(DOUBLE_PTR_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(DOUBLE_PTR_LL) -o $(DOUBLE_PTR_OBJ)

$(FILL_OBJ): $(FILL_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(FILL_LL) -o $(FILL_OBJ)

$(QUICK_SORT_OBJ): $(QUICK_SORT_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(QUICK_SORT_LL) -o $(QUICK_SORT_OBJ)

$(MERGE_SORT_OBJ): $(MERGE_SORT_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(MERGE_SORT_LL) -o $(MERGE_SORT_OBJ)

$(HEAP_SORT_OBJ): $(HEAP_SORT_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(HEAP_SORT_LL) -o $(HEAP_SORT_OBJ)

$(REVERSE_STRING_OBJ): $(REVERSE_STRING_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(REVERSE_STRING_LL) -o $(REVERSE_STRING_OBJ)

$(LOOP_CONTROL_OBJ): $(LOOP_CONTROL_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(LOOP_CONTROL_LL) -o $(LOOP_CONTROL_OBJ)

$(PRIMES_OBJ): $(PRIMES_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(PRIMES_LL) -o $(PRIMES_OBJ)

$(BST_OBJ): $(BST_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(BST_LL) -o $(BST_OBJ)

$(SIEVE_OBJ): $(SIEVE_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(SIEVE_LL) -o $(SIEVE_OBJ)

$(GCD_OBJ): $(GCD_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(GCD_LL) -o $(GCD_OBJ)

$(CONV_OBJ): $(CONV_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(CONV_LL) -o $(CONV_OBJ)

$(STRUCT_OBJ): $(STRUCT_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(STRUCT_LL) -o $(STRUCT_OBJ)

$(STRUCT_LIST_OBJ): $(STRUCT_LIST_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(STRUCT_LIST_LL) -o $(STRUCT_LIST_OBJ)

$(EXTERN_OBJ): $(EXTERN_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(EXTERN_LL) -o $(EXTERN_OBJ)

$(EXTERN_IO_OBJ): $(EXTERN_IO_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(EXTERN_IO_LL) -o $(EXTERN_IO_OBJ)

$(ENUM_OBJ): $(ENUM_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(ENUM_LL) -o $(ENUM_OBJ)

$(STATIC_OBJ): $(STATIC_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(STATIC_LL) -o $(STATIC_OBJ)

$(COMPLEX_OBJ): $(COMPLEX_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(COMPLEX_LL) -o $(COMPLEX_OBJ)

$(SIZEOF_OBJ): $(SIZEOF_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(SIZEOF_LL) -o $(SIZEOF_OBJ)

$(BIN): $(OBJ) $(DRIVER)
	$(CC) $(CFLAGS) -o $(BIN) $(DRIVER) $(OBJ)

$(FIB_BIN): $(FIB_OBJ) $(FIB_DRIVER)
	$(CC) $(CFLAGS) -o $(FIB_BIN) $(FIB_DRIVER) $(FIB_OBJ)

$(FOR_BIN): $(FOR_OBJ) $(FOR_DRIVER)
	$(CC) $(CFLAGS) -o $(FOR_BIN) $(FOR_DRIVER) $(FOR_OBJ)

$(SWAP_BIN): $(SWAP_OBJ) $(SWAP_DRIVER)
	$(CC) $(CFLAGS) -o $(SWAP_BIN) $(SWAP_DRIVER) $(SWAP_OBJ)

$(DOUBLE_PTR_BIN): $(DOUBLE_PTR_OBJ) $(DOUBLE_PTR_DRIVER)
	$(CC) $(CFLAGS) -o $(DOUBLE_PTR_BIN) $(DOUBLE_PTR_DRIVER) \
		$(DOUBLE_PTR_OBJ)

$(FILL_BIN): $(FILL_OBJ) $(FILL_DRIVER)
	$(CC) $(CFLAGS) -o $(FILL_BIN) $(FILL_DRIVER) $(FILL_OBJ)

$(QUICK_SORT_BIN): $(QUICK_SORT_OBJ) $(QUICK_SORT_DRIVER)
	$(CC) $(CFLAGS) -o $(QUICK_SORT_BIN) $(QUICK_SORT_DRIVER) \
		$(QUICK_SORT_OBJ)

$(MERGE_SORT_BIN): $(MERGE_SORT_OBJ) $(MERGE_SORT_DRIVER)
	$(CC) $(CFLAGS) -o $(MERGE_SORT_BIN) $(MERGE_SORT_DRIVER) \
		$(MERGE_SORT_OBJ)

$(HEAP_SORT_BIN): $(HEAP_SORT_OBJ) $(HEAP_SORT_DRIVER)
	$(CC) $(CFLAGS) -o $(HEAP_SORT_BIN) $(HEAP_SORT_DRIVER) \
		$(HEAP_SORT_OBJ)

$(REVERSE_STRING_BIN): $(REVERSE_STRING_OBJ) $(REVERSE_STRING_DRIVER)
	$(CC) $(CFLAGS) -o $(REVERSE_STRING_BIN) $(REVERSE_STRING_DRIVER) \
		$(REVERSE_STRING_OBJ)

$(LOOP_CONTROL_BIN): $(LOOP_CONTROL_OBJ) $(LOOP_CONTROL_DRIVER)
	$(CC) $(CFLAGS) -o $(LOOP_CONTROL_BIN) $(LOOP_CONTROL_DRIVER) \
		$(LOOP_CONTROL_OBJ)

$(PRIMES_BIN): $(PRIMES_OBJ) $(PRIMES_DRIVER)
	$(CC) $(CFLAGS) -o $(PRIMES_BIN) $(PRIMES_DRIVER) $(PRIMES_OBJ)

$(BST_BIN): $(BST_OBJ) $(BST_DRIVER)
	$(CC) $(CFLAGS) -o $(BST_BIN) $(BST_DRIVER) $(BST_OBJ)

$(SIEVE_BIN): $(SIEVE_OBJ) $(SIEVE_DRIVER)
	$(CC) $(CFLAGS) -o $(SIEVE_BIN) $(SIEVE_DRIVER) $(SIEVE_OBJ)

$(GCD_BIN): $(GCD_OBJ) $(GCD_DRIVER)
	$(CC) $(CFLAGS) -o $(GCD_BIN) $(GCD_DRIVER) $(GCD_OBJ)

$(CONV_BIN): $(CONV_OBJ) $(CONV_DRIVER)
	$(CC) $(CFLAGS) -o $(CONV_BIN) $(CONV_DRIVER) $(CONV_OBJ)

$(STRUCT_BIN): $(STRUCT_OBJ) $(STRUCT_DRIVER)
	$(CC) $(CFLAGS) -o $(STRUCT_BIN) $(STRUCT_DRIVER) $(STRUCT_OBJ)

$(STRUCT_LIST_BIN): $(STRUCT_LIST_OBJ) $(STRUCT_LIST_DRIVER)
	$(CC) $(CFLAGS) -o $(STRUCT_LIST_BIN) $(STRUCT_LIST_DRIVER) \
		$(STRUCT_LIST_OBJ)

$(EXTERN_BIN): $(EXTERN_OBJ) $(EXTERN_DRIVER)
	$(CC) $(CFLAGS) -o $(EXTERN_BIN) $(EXTERN_DRIVER) $(EXTERN_OBJ)

$(EXTERN_IO_BIN): $(EXTERN_IO_OBJ) $(EXTERN_IO_DRIVER)
	$(CC) $(CFLAGS) -o $(EXTERN_IO_BIN) $(EXTERN_IO_DRIVER) $(EXTERN_IO_OBJ)

$(ENUM_BIN): $(ENUM_OBJ) $(ENUM_DRIVER)
	$(CC) $(CFLAGS) -o $(ENUM_BIN) $(ENUM_DRIVER) $(ENUM_OBJ)

$(STATIC_BIN): $(STATIC_OBJ) $(STATIC_DRIVER)
	$(CC) $(CFLAGS) -o $(STATIC_BIN) $(STATIC_DRIVER) $(STATIC_OBJ)

$(COMPLEX_BIN): $(COMPLEX_OBJ) $(COMPLEX_DRIVER)
	$(CC) $(CFLAGS) -o $(COMPLEX_BIN) $(COMPLEX_DRIVER) $(COMPLEX_OBJ)

$(SIZEOF_BIN): $(SIZEOF_OBJ) $(SIZEOF_DRIVER)
	$(CC) $(CFLAGS) -o $(SIZEOF_BIN) $(SIZEOF_DRIVER) $(SIZEOF_OBJ)

$(OUTPUT): $(BIN)
	./$(BIN) > $(OUTPUT)

$(FIB_OUTPUT): $(FIB_BIN)
	./$(FIB_BIN) > $(FIB_OUTPUT)

$(FOR_OUTPUT): $(FOR_BIN)
	./$(FOR_BIN) > $(FOR_OUTPUT)

$(SWAP_OUTPUT): $(SWAP_BIN)
	./$(SWAP_BIN) > $(SWAP_OUTPUT)

$(DOUBLE_PTR_OUTPUT): $(DOUBLE_PTR_BIN)
	./$(DOUBLE_PTR_BIN) > $(DOUBLE_PTR_OUTPUT)

$(FILL_OUTPUT): $(FILL_BIN)
	./$(FILL_BIN) > $(FILL_OUTPUT)

$(QUICK_SORT_OUTPUT): $(QUICK_SORT_BIN)
	./$(QUICK_SORT_BIN) > $(QUICK_SORT_OUTPUT)

$(MERGE_SORT_OUTPUT): $(MERGE_SORT_BIN)
	./$(MERGE_SORT_BIN) > $(MERGE_SORT_OUTPUT)

$(HEAP_SORT_OUTPUT): $(HEAP_SORT_BIN)
	./$(HEAP_SORT_BIN) > $(HEAP_SORT_OUTPUT)

$(REVERSE_STRING_OUTPUT): $(REVERSE_STRING_BIN)
	./$(REVERSE_STRING_BIN) > $(REVERSE_STRING_OUTPUT)

$(LOOP_CONTROL_OUTPUT): $(LOOP_CONTROL_BIN)
	./$(LOOP_CONTROL_BIN) > $(LOOP_CONTROL_OUTPUT)

$(PRIMES_OUTPUT): $(PRIMES_BIN)
	./$(PRIMES_BIN) > $(PRIMES_OUTPUT)

$(BST_OUTPUT): $(BST_BIN)
	./$(BST_BIN) > $(BST_OUTPUT)

$(SIEVE_OUTPUT): $(SIEVE_BIN)
	./$(SIEVE_BIN) > $(SIEVE_OUTPUT)

$(GCD_OUTPUT): $(GCD_BIN)
	./$(GCD_BIN) > $(GCD_OUTPUT)

$(CONV_OUTPUT): $(CONV_BIN)
	./$(CONV_BIN) > $(CONV_OUTPUT)

$(STRUCT_OUTPUT): $(STRUCT_BIN)
	./$(STRUCT_BIN) > $(STRUCT_OUTPUT)

$(STRUCT_LIST_OUTPUT): $(STRUCT_LIST_BIN)
	./$(STRUCT_LIST_BIN) > $(STRUCT_LIST_OUTPUT)

$(EXTERN_OUTPUT): $(EXTERN_BIN)
	./$(EXTERN_BIN) > $(EXTERN_OUTPUT)

$(ENUM_OUTPUT): $(ENUM_BIN)
	./$(ENUM_BIN) > $(ENUM_OUTPUT)

$(STATIC_OUTPUT): $(STATIC_BIN)
	./$(STATIC_BIN) > $(STATIC_OUTPUT)

$(COMPLEX_OUTPUT): $(COMPLEX_BIN)
	./$(COMPLEX_BIN) > $(COMPLEX_OUTPUT)

$(SIZEOF_OUTPUT): $(SIZEOF_BIN)
	./$(SIZEOF_BIN) > $(SIZEOF_OUTPUT)

$(EXTERN_IO_OUTPUT): $(EXTERN_IO_BIN)
	printf "input" | ./$(EXTERN_IO_BIN) > $(EXTERN_IO_OUTPUT) \
		2> $(EXTERN_IO_ERR_OUTPUT)

clean:
	rm -rf $(BUILD_DIR)
