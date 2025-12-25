CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2
CPPFLAGS ?= -Iinclude -I../02_parser/include -I../01_lexer/include -I../tests

BUILD_DIR := build
SRC := src/checker.c
OBJ := $(BUILD_DIR)/checker.o
LIB := $(BUILD_DIR)/libchecker.a

PARSER_DIR := ../02_parser
PARSER_LIB := $(PARSER_DIR)/build/libparser.a
LEXER_DIR := ../01_lexer
LEXER_LIB := $(LEXER_DIR)/build/liblexer.a

TEST_SRC := tests/test_checker.c
TEST_BIN := $(BUILD_DIR)/test_checker

EXAMPLE_SRC := examples/main_checker.c
EXAMPLE_BIN := $(BUILD_DIR)/main_checker

.PHONY: all test example clean

all: $(LIB)

test: $(TEST_BIN)
	./$(TEST_BIN)

example: $(EXAMPLE_BIN)
	./$(EXAMPLE_BIN)

$(PARSER_LIB):
	$(MAKE) -C $(PARSER_DIR) all

$(LEXER_LIB):
	$(MAKE) -C $(LEXER_DIR) all

$(TEST_BIN): $(TEST_SRC) $(LIB) $(PARSER_LIB) $(LEXER_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(TEST_SRC) $(LIB) \
		$(PARSER_LIB) $(LEXER_LIB)

$(EXAMPLE_BIN): $(EXAMPLE_SRC) $(LIB) $(PARSER_LIB) $(LEXER_LIB)
	$(CC) $(CPPFLAGS) $(CFLAGS) -o $@ $(EXAMPLE_SRC) $(LIB) \
		$(PARSER_LIB) $(LEXER_LIB)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(LIB): $(OBJ) | $(BUILD_DIR)
	ar rcs $@ $(OBJ)

$(OBJ): $(SRC) | $(BUILD_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $(SRC)

clean:
	rm -rf $(BUILD_DIR)
