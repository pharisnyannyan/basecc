CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -Iinclude

BUILD_DIR := build
SRC := src/lexer.c
OBJ := $(BUILD_DIR)/lexer.o

TEST_SRC := tests/test_lexer.c
TEST_BIN := $(BUILD_DIR)/test_lexer

.PHONY: all test clean

all: $(OBJ)

test: $(TEST_BIN)
	./$(TEST_BIN)

$(TEST_BIN): $(TEST_SRC) $(OBJ)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(TEST_SRC) $(OBJ)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ): $(SRC) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $(SRC)

clean:
	rm -rf $(BUILD_DIR)
