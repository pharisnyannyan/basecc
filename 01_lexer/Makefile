CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -Iinclude -I../tests

BUILD_DIR := build
SRC := src/lexer.c
OBJ := $(BUILD_DIR)/lexer.o
LIB := $(BUILD_DIR)/liblexer.a

TEST_SRC := tests/test_lexer.c
TEST_UTIL_SRC := ../tests/test_util.c
TEST_BIN := $(BUILD_DIR)/test_lexer

EXAMPLE_SRC := examples/main_lex.c
EXAMPLE_BIN := $(BUILD_DIR)/main_lex

.PHONY: all test example clean

all: $(LIB)

test: $(TEST_BIN)
	./$(TEST_BIN)

example: $(EXAMPLE_BIN)
	./$(EXAMPLE_BIN)

$(TEST_BIN): $(TEST_SRC) $(TEST_UTIL_SRC) $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(TEST_SRC) $(TEST_UTIL_SRC) $(LIB)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(EXAMPLE_SRC) $(LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $(OBJ)

$(OBJ): $(SRC) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $(SRC)

clean:
	rm -rf $(BUILD_DIR)
