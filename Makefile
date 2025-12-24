CC ?= cc
CFLAGS ?= -std=c11 -Wall -Wextra -Werror -O2

.PHONY: all test clean

all:
	$(MAKE) -C 01_lexer all

test:
	$(MAKE) -C 01_lexer test

clean:
	$(MAKE) -C 01_lexer clean
