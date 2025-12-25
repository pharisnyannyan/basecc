CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

.PHONY: all test clean

all:
	$(MAKE) -C 01_lexer all
	$(MAKE) -C 02_parser all
	$(MAKE) -C 03_checker all
	$(MAKE) -C 04_codegen all

test:
	$(MAKE) -C 01_lexer test
	$(MAKE) -C 02_parser test
	$(MAKE) -C 03_checker test
	$(MAKE) -C 04_codegen test

clean:
	$(MAKE) -C 01_lexer clean
	$(MAKE) -C 02_parser clean
	$(MAKE) -C 03_checker clean
	$(MAKE) -C 04_codegen clean
