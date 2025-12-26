#ifndef BASECC_CHECKER_H
#define BASECC_CHECKER_H

#include "parser.h"

typedef struct Checker {
    Parser parser;
    const char *error_message;
    int loop_depth;
} Checker;

void checker_init(Checker *checker, const char *input);
int checker_check(Checker *checker);
const char *checker_error(const Checker *checker);

#endif
