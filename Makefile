CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

.PHONY: all test clean

all:
	$(MAKE) -C 01_lexer all
	$(MAKE) -C 02_parser all

test:
	$(MAKE) -C 01_lexer test
	$(MAKE) -C 02_parser test

clean:
	$(MAKE) -C 01_lexer clean
	$(MAKE) -C 02_parser clean
