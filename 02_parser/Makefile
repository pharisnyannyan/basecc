CC ?= clang
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -Iinclude -I../01_lexer/include -I../tests

BUILD_DIR := build
SRC := src/parser.c
OBJ := $(BUILD_DIR)/parser.o
LIB := $(BUILD_DIR)/libparser.a

LEXER_DIR := ../01_lexer
LEXER_LIB := $(LEXER_DIR)/build/liblexer.a

TEST_SRC := tests/test_parser.c
TEST_UTIL_SRC := ../tests/test_util.c
TEST_BIN := $(BUILD_DIR)/test_parser

EXAMPLE_SRC := examples/main_parser.c
EXAMPLE_BIN := $(BUILD_DIR)/main_parser

.PHONY: all test example clean

all: $(LIB)

test: $(TEST_BIN)
	./$(TEST_BIN)

example: $(EXAMPLE_BIN)
	./$(EXAMPLE_BIN)

$(LEXER_LIB):
	$(MAKE) -C $(LEXER_DIR) all

$(TEST_BIN): $(TEST_SRC) $(TEST_UTIL_SRC) $(LIB) $(LEXER_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(TEST_SRC) $(TEST_UTIL_SRC) \
		$(LIB) $(LEXER_LIB)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB) $(LEXER_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(EXAMPLE_SRC) $(LIB) $(LEXER_LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $(OBJ)

$(OBJ): $(SRC) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $(SRC)

clean:
	rm -rf $(BUILD_DIR)
