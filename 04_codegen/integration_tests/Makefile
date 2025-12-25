CC ?= clang
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -I../include -I../../03_checker/include -I../../02_parser/include \
	-I../../01_lexer/include

BUILD_DIR := build
LL := ../build/codegen_arithmetic.ll
INPUT := ../testdata/arithmetic_runtime.c
EXPECTED := arithmetic_driver_expected.txt

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

.PHONY: all compile generate run verify clean

all: $(BIN)

generate: $(CODEGEN_BIN)
	./$(CODEGEN_BIN) $(INPUT) $(LL)

compile: generate $(OBJ)

run: all
	./$(BIN) > $(OUTPUT)

verify: run
	cmp -s $(OUTPUT) $(EXPECTED)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(CODEGEN_BIN): $(CODEGEN_SRC) $(CODEGEN_LIB) $(CHECKER_LIB) \
	$(PARSER_LIB) $(LEXER_LIB) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(CODEGEN_SRC) $(CODEGEN_LIB) \
		$(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)

$(OBJ): $(LL) | $(BUILD_DIR)
	$(CC) -c $(LL) -o $(OBJ)

$(BIN): $(OBJ) $(DRIVER)
	$(CC) $(CFLAGS) -o $(BIN) $(DRIVER) $(OBJ)

clean:
	rm -rf $(BUILD_DIR)
