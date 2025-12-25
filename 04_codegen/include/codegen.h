#ifndef BASECC_CODEGEN_H
#define BASECC_CODEGEN_H

#include "checker.h"

typedef struct Codegen {
    const char *input;
    Checker checker;
    Parser parser;
    const char *error_message;
} Codegen;

void codegen_init(Codegen *codegen, const char *input);
int codegen_emit(Codegen *codegen, const char *output_path);
const char *codegen_error(const Codegen *codegen);

#endif
