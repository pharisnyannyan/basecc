#include "codegen.h"

#include <stdio.h>
#include <string.h>

typedef struct FunctionContext {
    Codegen *codegen;
    FILE *out;
    int next_label_id;
    int next_temp_id;
} FunctionContext;

static int token_is_punct(Token token, const char *text)
{
    size_t length = strlen(text);

    if (token.type != TOKEN_PUNCT) {
        return 0;
    }

    if (token.length != length) {
        return 0;
    }

    return strncmp(token.start, text, length) == 0;
}

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

static int codegen_emit_expression(FunctionContext *ctx,
    const ParserNode *node,
    char *value,
    size_t value_size)
{
    if (node->type == PARSER_NODE_NUMBER) {
        if (node->token.type != TOKEN_NUMBER) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected number token");
        }

        if (node->first_child) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected expression child");
        }

        snprintf(value, value_size, "%ld", node->token.value);
        return 1;
    }

    if (node->type == PARSER_NODE_CALL) {
        if (node->token.type != TOKEN_IDENT) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected call identifier");
        }

        if (node->first_child) {
            return codegen_set_error(ctx->codegen,
                "codegen: arguments not supported");
        }

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = call i32 @%.*s()\n",
            value,
            (int)node->token.length,
            node->token.start);
        return 1;
    }

    if (node->type == PARSER_NODE_BINARY) {
        const ParserNode *left = node->first_child;
        const ParserNode *right = left ? left->next : NULL;
        char left_value[32];
        char right_value[32];
        const char *opcode = NULL;

        if (!left || !right || right->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected binary operands");
        }

        if (!codegen_emit_expression(ctx, left, left_value,
            sizeof(left_value))) {
            return 0;
        }

        if (!codegen_emit_expression(ctx, right, right_value,
            sizeof(right_value))) {
            return 0;
        }

        if (token_is_punct(node->token, "+")) {
            opcode = "add";
        } else if (token_is_punct(node->token, "-")) {
            opcode = "sub";
        } else if (token_is_punct(node->token, "*")) {
            opcode = "mul";
        } else if (token_is_punct(node->token, "/")) {
            opcode = "sdiv";
        } else if (token_is_punct(node->token, "%")) {
            opcode = "srem";
        } else {
            return codegen_set_error(ctx->codegen,
                "codegen: expected binary operator");
        }

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = %s i32 %s, %s\n",
            value, opcode, left_value, right_value);
        return 1;
    }

    return codegen_set_error(ctx->codegen,
        "codegen: expected expression");
}

static void codegen_format_label(char *buffer, size_t size, const char *prefix,
    int id)
{
    snprintf(buffer, size, "%s%d", prefix, id);
}

static int codegen_emit_statement(FunctionContext *ctx, const ParserNode *node);

static int codegen_emit_block(FunctionContext *ctx, const ParserNode *node)
{
    const ParserNode *child = NULL;
    int terminated = 0;

    if (node->type != PARSER_NODE_BLOCK) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected block");
    }

    for (child = node->first_child; child; child = child->next) {
        if (terminated) {
            break;
        }

        terminated = codegen_emit_statement(ctx, child);
        if (ctx->codegen->error_message) {
            return 0;
        }
    }

    return terminated;
}

static int codegen_emit_if(FunctionContext *ctx, const ParserNode *node)
{
    const ParserNode *condition = node->first_child;
    const ParserNode *then_branch = condition ? condition->next : NULL;
    const ParserNode *else_branch = then_branch ? then_branch->next : NULL;
    char value[32];
    char temp[32];
    char then_label[32];
    char else_label[32];
    char end_label[32];
    int then_terminated = 0;
    int else_terminated = 0;
    int need_end = 1;

    if (!condition || !then_branch) {
        return codegen_set_error(ctx->codegen,
            "codegen: incomplete if statement");
    }

    if (else_branch && else_branch->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected else statement");
    }

    if (!codegen_emit_expression(ctx, condition, value, sizeof(value))) {
        return 0;
    }

    snprintf(temp, sizeof(temp), "%%t%d", ctx->next_temp_id++);
    codegen_format_label(then_label, sizeof(then_label), "if.then",
        ctx->next_label_id++);

    if (else_branch) {
        codegen_format_label(else_label, sizeof(else_label), "if.else",
            ctx->next_label_id++);
        codegen_format_label(end_label, sizeof(end_label), "if.end",
            ctx->next_label_id++);
    } else {
        codegen_format_label(end_label, sizeof(end_label), "if.end",
            ctx->next_label_id++);
        strncpy(else_label, end_label, sizeof(else_label));
        else_label[sizeof(else_label) - 1] = '\0';
    }

    fprintf(ctx->out, "  %s = icmp ne i32 %s, 0\n", temp, value);
    fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
        temp,
        then_label,
        else_label);

    fprintf(ctx->out, "%s:\n", then_label);
    then_terminated = codegen_emit_statement(ctx, then_branch);
    if (ctx->codegen->error_message) {
        return 0;
    }
    if (!then_terminated) {
        fprintf(ctx->out, "  br label %%%s\n", end_label);
    }

    if (else_branch) {
        fprintf(ctx->out, "%s:\n", else_label);
        else_terminated = codegen_emit_statement(ctx, else_branch);
        if (ctx->codegen->error_message) {
            return 0;
        }
        if (!else_terminated) {
            fprintf(ctx->out, "  br label %%%s\n", end_label);
        }

        if (then_terminated && else_terminated) {
            need_end = 0;
        }
    }

    if (need_end) {
        fprintf(ctx->out, "%s:\n", end_label);
        return 0;
    }

    return 1;
}

static int codegen_emit_while(FunctionContext *ctx, const ParserNode *node)
{
    const ParserNode *condition = node->first_child;
    const ParserNode *body = condition ? condition->next : NULL;
    char value[32];
    char temp[32];
    char cond_label[32];
    char body_label[32];
    char end_label[32];
    int body_terminated = 0;

    if (!condition || !body) {
        return codegen_set_error(ctx->codegen,
            "codegen: incomplete while statement");
    }

    if (body->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected while statement");
    }

    if (!codegen_emit_expression(ctx, condition, value, sizeof(value))) {
        return 0;
    }

    codegen_format_label(cond_label, sizeof(cond_label), "while.cond",
        ctx->next_label_id++);
    codegen_format_label(body_label, sizeof(body_label), "while.body",
        ctx->next_label_id++);
    codegen_format_label(end_label, sizeof(end_label), "while.end",
        ctx->next_label_id++);

    fprintf(ctx->out, "  br label %%%s\n", cond_label);
    fprintf(ctx->out, "%s:\n", cond_label);

    snprintf(temp, sizeof(temp), "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out, "  %s = icmp ne i32 %s, 0\n", temp, value);
    fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
        temp,
        body_label,
        end_label);

    fprintf(ctx->out, "%s:\n", body_label);
    body_terminated = codegen_emit_statement(ctx, body);
    if (ctx->codegen->error_message) {
        return 0;
    }
    if (!body_terminated) {
        fprintf(ctx->out, "  br label %%%s\n", cond_label);
    }

    fprintf(ctx->out, "%s:\n", end_label);
    return 0;
}

static int codegen_emit_statement(FunctionContext *ctx, const ParserNode *node)
{
    char value[32];

    switch (node->type) {
    case PARSER_NODE_BLOCK:
        return codegen_emit_block(ctx, node);
    case PARSER_NODE_IF:
        return codegen_emit_if(ctx, node);
    case PARSER_NODE_WHILE:
        return codegen_emit_while(ctx, node);
    case PARSER_NODE_RETURN:
        if (!node->first_child || node->first_child->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected return statement");
        }

        if (!codegen_emit_expression(ctx, node->first_child, value,
            sizeof(value))) {
            return 0;
        }

        fprintf(ctx->out, "  ret i32 %s\n", value);
        return 1;
    case PARSER_NODE_EMPTY:
        if (node->first_child) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected empty statement");
        }
        return 0;
    default:
        return codegen_set_error(ctx->codegen,
            "codegen: expected statement");
    }
}

static int codegen_emit_function(Codegen *codegen, const ParserNode *node,
    FILE *out)
{
    FunctionContext ctx;
    int terminated = 0;

    if (node->type != PARSER_NODE_FUNCTION) {
        return codegen_set_error(codegen,
            "codegen: expected function");
    }

    if (node->token.type != TOKEN_IDENT) {
        return codegen_set_error(codegen,
            "codegen: expected identifier token");
    }

    if (!node->first_child || node->first_child->next) {
        return codegen_set_error(codegen,
            "codegen: expected function body");
    }

    if (node->first_child->type != PARSER_NODE_BLOCK) {
        return codegen_set_error(codegen,
            "codegen: expected function block");
    }

    ctx.codegen = codegen;
    ctx.out = out;
    ctx.next_label_id = 0;
    ctx.next_temp_id = 0;

    fprintf(out, "define i32 @%.*s() {\n",
        (int)node->token.length,
        node->token.start);
    fprintf(out, "entry:\n");

    terminated = codegen_emit_block(&ctx, node->first_child);
    if (codegen->error_message) {
        return 0;
    }

    if (!terminated) {
        fprintf(out, "  ret i32 0\n");
    }

    fprintf(out, "}\n");
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
        if (child->type == PARSER_NODE_DECLARATION) {
            if (!codegen_emit_declaration(codegen, child, out)) {
                return 0;
            }
            continue;
        }

        if (child->type == PARSER_NODE_FUNCTION) {
            if (!codegen_emit_function(codegen, child, out)) {
                return 0;
            }
            continue;
        }

        return codegen_set_error(codegen,
            "codegen: unexpected top-level node");
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
