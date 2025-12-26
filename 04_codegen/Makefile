CC ?= clang
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -Iinclude -I../03_checker/include -I../02_parser/include \
	-I../01_lexer/include -I../tests

BUILD_DIR := build
SRC := src/codegen.c
OBJ := $(BUILD_DIR)/codegen.o
LIB := $(BUILD_DIR)/libcodegen.a

CHECKER_DIR := ../03_checker
CHECKER_LIB := $(CHECKER_DIR)/build/libchecker.a
PARSER_DIR := ../02_parser
PARSER_LIB := $(PARSER_DIR)/build/libparser.a
LEXER_DIR := ../01_lexer
LEXER_LIB := $(LEXER_DIR)/build/liblexer.a

TEST_SRC := tests/test_codegen.c
TEST_UTIL_SRC := ../tests/test_util.c
TEST_BIN := $(BUILD_DIR)/test_codegen

EXAMPLE_SRC := examples/main_codegen.c
EXAMPLE_BIN := $(BUILD_DIR)/main_codegen

.PHONY: all test example integration-test clean

all: $(LIB)

test: $(TEST_BIN)
	./$(TEST_BIN)

example: $(EXAMPLE_BIN)
	./$(EXAMPLE_BIN)

integration-test: $(LIB) $(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)
	$(MAKE) -C integration_tests verify

$(CHECKER_LIB):
	$(MAKE) -C $(CHECKER_DIR) all

$(PARSER_LIB):
	$(MAKE) -C $(PARSER_DIR) all

$(LEXER_LIB):
	$(MAKE) -C $(LEXER_DIR) all

$(TEST_BIN): $(TEST_SRC) $(TEST_UTIL_SRC) $(LIB) \
	$(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(TEST_SRC) $(TEST_UTIL_SRC) $(LIB) \
		$(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB) $(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(EXAMPLE_SRC) $(LIB) \
		$(CHECKER_LIB) $(PARSER_LIB) $(LEXER_LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $(OBJ)

$(OBJ): $(SRC) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $(SRC)

clean:
	rm -rf $(BUILD_DIR)
