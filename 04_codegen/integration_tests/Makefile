CC ?= cc
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
FILL_LL := ../build/codegen_fill_array.ll
FILL_INPUT := testdata/fill_array.c
FILL_EXPECTED := fill_array_driver_expected.txt
QUICK_SORT_LL := ../build/codegen_quick_sort.ll
QUICK_SORT_INPUT := testdata/quick_sort.c
QUICK_SORT_EXPECTED := quick_sort_driver_expected.txt

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
FILL_OBJ := $(BUILD_DIR)/fill_array.o
FILL_BIN := $(BUILD_DIR)/fill_array_driver
FILL_DRIVER := fill_array_driver.c
FILL_OUTPUT := $(BUILD_DIR)/fill_array_output.txt
QUICK_SORT_OBJ := $(BUILD_DIR)/quick_sort.o
QUICK_SORT_BIN := $(BUILD_DIR)/quick_sort_driver
QUICK_SORT_DRIVER := quick_sort_driver.c
QUICK_SORT_OUTPUT := $(BUILD_DIR)/quick_sort_output.txt

.PHONY: all compile generate run verify clean

all: $(BIN) $(FIB_BIN) $(FOR_BIN) $(SWAP_BIN) $(FILL_BIN) $(QUICK_SORT_BIN)

generate: $(LL) $(FIB_LL) $(FOR_LL) $(SWAP_LL) $(FILL_LL) $(QUICK_SORT_LL)

$(LL): $(CODEGEN_BIN) $(INPUT)
	./$(CODEGEN_BIN) $(INPUT) $(LL)

$(FIB_LL): $(CODEGEN_BIN) $(FIB_INPUT)
	./$(CODEGEN_BIN) $(FIB_INPUT) $(FIB_LL)

$(FOR_LL): $(CODEGEN_BIN) $(FOR_INPUT)
	./$(CODEGEN_BIN) $(FOR_INPUT) $(FOR_LL)

$(SWAP_LL): $(CODEGEN_BIN) $(SWAP_INPUT)
	./$(CODEGEN_BIN) $(SWAP_INPUT) $(SWAP_LL)

$(FILL_LL): $(CODEGEN_BIN) $(FILL_INPUT)
	./$(CODEGEN_BIN) $(FILL_INPUT) $(FILL_LL)

$(QUICK_SORT_LL): $(CODEGEN_BIN) $(QUICK_SORT_INPUT)
	./$(CODEGEN_BIN) $(QUICK_SORT_INPUT) $(QUICK_SORT_LL)

compile: generate $(OBJ) $(FIB_OBJ) $(FOR_OBJ) $(SWAP_OBJ) $(FILL_OBJ) \
	$(QUICK_SORT_OBJ)

run: all $(OUTPUT) $(FIB_OUTPUT) $(FOR_OUTPUT) $(SWAP_OUTPUT) \
	$(FILL_OUTPUT) $(QUICK_SORT_OUTPUT)

verify: run
	cmp -s $(OUTPUT) $(EXPECTED)
	cmp -s $(FIB_OUTPUT) $(FIB_EXPECTED)
	cmp -s $(FOR_OUTPUT) $(FOR_EXPECTED)
	cmp -s $(SWAP_OUTPUT) $(SWAP_EXPECTED)
	cmp -s $(FILL_OUTPUT) $(FILL_EXPECTED)
	cmp -s $(QUICK_SORT_OUTPUT) $(QUICK_SORT_EXPECTED)

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

$(FILL_OBJ): $(FILL_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(FILL_LL) -o $(FILL_OBJ)

$(QUICK_SORT_OBJ): $(QUICK_SORT_LL) | $(BUILD_DIR)
	$(LL_CC) -c $(QUICK_SORT_LL) -o $(QUICK_SORT_OBJ)

$(BIN): $(OBJ) $(DRIVER)
	$(CC) $(CFLAGS) -o $(BIN) $(DRIVER) $(OBJ)

$(FIB_BIN): $(FIB_OBJ) $(FIB_DRIVER)
	$(CC) $(CFLAGS) -o $(FIB_BIN) $(FIB_DRIVER) $(FIB_OBJ)

$(FOR_BIN): $(FOR_OBJ) $(FOR_DRIVER)
	$(CC) $(CFLAGS) -o $(FOR_BIN) $(FOR_DRIVER) $(FOR_OBJ)

$(SWAP_BIN): $(SWAP_OBJ) $(SWAP_DRIVER)
	$(CC) $(CFLAGS) -o $(SWAP_BIN) $(SWAP_DRIVER) $(SWAP_OBJ)

$(FILL_BIN): $(FILL_OBJ) $(FILL_DRIVER)
	$(CC) $(CFLAGS) -o $(FILL_BIN) $(FILL_DRIVER) $(FILL_OBJ)

$(QUICK_SORT_BIN): $(QUICK_SORT_OBJ) $(QUICK_SORT_DRIVER)
	$(CC) $(CFLAGS) -o $(QUICK_SORT_BIN) $(QUICK_SORT_DRIVER) \
		$(QUICK_SORT_OBJ)

$(OUTPUT): $(BIN)
	./$(BIN) > $(OUTPUT)

$(FIB_OUTPUT): $(FIB_BIN)
	./$(FIB_BIN) > $(FIB_OUTPUT)

$(FOR_OUTPUT): $(FOR_BIN)
	./$(FOR_BIN) > $(FOR_OUTPUT)

$(SWAP_OUTPUT): $(SWAP_BIN)
	./$(SWAP_BIN) > $(SWAP_OUTPUT)

$(FILL_OUTPUT): $(FILL_BIN)
	./$(FILL_BIN) > $(FILL_OUTPUT)

$(QUICK_SORT_OUTPUT): $(QUICK_SORT_BIN)
	./$(QUICK_SORT_BIN) > $(QUICK_SORT_OUTPUT)

clean:
	rm -rf $(BUILD_DIR)
