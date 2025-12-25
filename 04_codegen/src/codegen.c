#include "codegen.h"

#include <stdio.h>

static int codegen_set_error(Codegen *codegen, const char *message)
{
    if (!codegen->error_message) {
        codegen->error_message = message;
    }

    return 0;
}

void codegen_init(Codegen *codegen, const char *input)
{
    codegen->input = input;
    codegen->error_message = NULL;
    checker_init(&codegen->checker, input);
    parser_init(&codegen->parser, input);
}

static int codegen_emit_declaration(Codegen *codegen, const ParserNode *node,
    FILE *out)
{
    long value = 0;

    if (node->type != PARSER_NODE_DECLARATION) {
        return codegen_set_error(codegen,
            "codegen: expected declaration node");
    }

    if (node->token.type != TOKEN_IDENT) {
        return codegen_set_error(codegen,
            "codegen: expected identifier token");
    }

    if (node->first_child) {
        if (node->first_child->next) {
            return codegen_set_error(codegen,
                "codegen: unexpected initializer list");
        }

        if (node->first_child->type != PARSER_NODE_NUMBER) {
            return codegen_set_error(codegen,
                "codegen: expected number initializer");
        }

        value = node->first_child->token.value;
    }

    fprintf(out, "@%.*s = global i32 %ld\n",
        (int)node->token.length,
        node->token.start,
        value);

    return 1;
}

static int codegen_emit_translation_unit(Codegen *codegen,
    const ParserNode *node,
    FILE *out)
{
    const ParserNode *child = NULL;

    if (node->type != PARSER_NODE_TRANSLATION_UNIT) {
        return codegen_set_error(codegen,
            "codegen: expected translation unit");
    }

    if (node->token.type != TOKEN_EOF) {
        return codegen_set_error(codegen, "codegen: expected EOF token");
    }

    fprintf(out, "; ModuleID = 'basecc'\n");
    fprintf(out, "source_filename = \"basecc\"\n\n");

    for (child = node->first_child; child; child = child->next) {
        if (!codegen_emit_declaration(codegen, child, out)) {
            return 0;
        }
    }

    return 1;
}

int codegen_emit(Codegen *codegen, const char *output_path)
{
    ParserNode *root = NULL;
    const char *parser_message = NULL;
    FILE *out = NULL;

    codegen->error_message = NULL;

    checker_init(&codegen->checker, codegen->input);
    if (!checker_check(&codegen->checker)) {
        return codegen_set_error(codegen, checker_error(&codegen->checker));
    }

    parser_init(&codegen->parser, codegen->input);
    root = parser_parse(&codegen->parser);
    parser_message = parser_error(&codegen->parser);

    if (!root) {
        return codegen_set_error(codegen, "codegen: out of memory");
    }

    if (parser_message) {
        codegen_set_error(codegen, parser_message);
        parser_free_node(root);
        return 0;
    }

    out = fopen(output_path, "w");
    if (!out) {
        codegen_set_error(codegen, "codegen: failed to open output file");
        parser_free_node(root);
        return 0;
    }

    if (!codegen_emit_translation_unit(codegen, root, out)) {
        fclose(out);
        parser_free_node(root);
        return 0;
    }

    if (fclose(out) != 0) {
        parser_free_node(root);
        return codegen_set_error(codegen,
            "codegen: failed to write output file");
    }

    parser_free_node(root);
    return 1;
}

const char *codegen_error(const Codegen *codegen)
{
    return codegen->error_message;
}
