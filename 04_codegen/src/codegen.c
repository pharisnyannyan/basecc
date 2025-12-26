#include "codegen.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct FunctionContext {
    Codegen *codegen;
    FILE *out;
    int next_label_id;
    int next_temp_id;
    char return_type[32];
    int return_width;
    int return_pointer_depth;
    TokenType return_base_token;
    const struct GlobalSymbol *globals;
    size_t global_count;
    const ParserNode *params;
    size_t param_count;
    struct LocalSymbol *locals;
    size_t local_count;
    size_t local_capacity;
    const struct FunctionSymbol *functions;
    size_t function_count;
} FunctionContext;

typedef struct TypeInfo {
    const char *ir_name;
    int width;
} TypeInfo;

typedef struct TypeDesc {
    TokenType base_token;
    int pointer_depth;
} TypeDesc;

typedef struct GlobalSymbol {
    const char *name;
    size_t length;
    Token type_token;
    int pointer_depth;
} GlobalSymbol;

typedef struct FunctionSymbol {
    const char *name;
    size_t length;
    Token type_token;
    int pointer_depth;
    const ParserNode *param_list;
    size_t param_count;
} FunctionSymbol;

typedef struct LocalSymbol {
    const char *name;
    size_t length;
    Token type_token;
    int pointer_depth;
    char ir_name[32];
} LocalSymbol;

static int codegen_set_error(Codegen *codegen, const char *message);

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

static TypeInfo codegen_type_info(Token token)
{
    TypeInfo info = { "i32", 32 };

    if (token.type == TOKEN_CHAR) {
        info.ir_name = "i8";
        info.width = 8;
    } else if (token.type == TOKEN_SHORT) {
        info.ir_name = "i16";
        info.width = 16;
    }

    return info;
}

static TypeDesc codegen_make_type_desc(Token token, int pointer_depth)
{
    TypeDesc desc;

    desc.base_token = token.type;
    desc.pointer_depth = pointer_depth;
    return desc;
}

static TypeDesc codegen_int_type_desc(void)
{
    TypeDesc desc;

    desc.base_token = TOKEN_INT;
    desc.pointer_depth = 0;
    return desc;
}

static void codegen_format_type(Token token, int pointer_depth,
    char *buffer,
    size_t buffer_size)
{
    TypeInfo info = codegen_type_info(token);
    size_t length = 0;
    int i = 0;

    snprintf(buffer, buffer_size, "%s", info.ir_name);
    length = strlen(buffer);

    for (i = 0; i < pointer_depth && length + 1 < buffer_size; i++) {
        buffer[length++] = '*';
        buffer[length] = '\0';
    }
}

static void codegen_format_desc_type(TypeDesc desc, char *buffer,
    size_t buffer_size)
{
    Token token;

    token.type = desc.base_token;
    token.start = NULL;
    token.length = 0;
    token.value = 0;

    codegen_format_type(token, desc.pointer_depth, buffer, buffer_size);
}

static int codegen_type_is_integer(TypeDesc desc)
{
    return desc.pointer_depth == 0;
}

static int codegen_name_matches(Token token, const char *name, size_t length)
{
    if (token.length != length) {
        return 0;
    }

    return strncmp(token.start, name, length) == 0;
}

static const GlobalSymbol *codegen_find_global(const FunctionContext *ctx,
    Token name)
{
    size_t i = 0;

    for (i = 0; i < ctx->global_count; i++) {
        if (codegen_name_matches(name,
                ctx->globals[i].name,
                ctx->globals[i].length)) {
            return &ctx->globals[i];
        }
    }

    return NULL;
}

static const FunctionSymbol *codegen_find_function(const FunctionContext *ctx,
    Token name)
{
    size_t i = 0;

    for (i = 0; i < ctx->function_count; i++) {
        if (codegen_name_matches(name,
                ctx->functions[i].name,
                ctx->functions[i].length)) {
            return &ctx->functions[i];
        }
    }

    return NULL;
}

static const LocalSymbol *codegen_find_local(const FunctionContext *ctx,
    Token name)
{
    size_t i = 0;

    for (i = 0; i < ctx->local_count; i++) {
        if (codegen_name_matches(name,
                ctx->locals[i].name,
                ctx->locals[i].length)) {
            return &ctx->locals[i];
        }
    }

    return NULL;
}

static LocalSymbol *codegen_add_local(FunctionContext *ctx,
    const ParserNode *node)
{
    size_t new_capacity = 0;
    LocalSymbol *locals = NULL;
    LocalSymbol *symbol = NULL;

    if (ctx->local_count + 1 > ctx->local_capacity) {
        new_capacity = ctx->local_capacity ? ctx->local_capacity * 2 : 8;
        locals = realloc(ctx->locals, new_capacity * sizeof(*locals));
        if (!locals) {
            codegen_set_error(ctx->codegen, "codegen: out of memory");
            return NULL;
        }

        ctx->locals = locals;
        ctx->local_capacity = new_capacity;
    }

    symbol = &ctx->locals[ctx->local_count++];
    symbol->name = node->token.start;
    symbol->length = node->token.length;
    symbol->type_token = node->type_token;
    symbol->pointer_depth = node->pointer_depth;
    snprintf(symbol->ir_name, sizeof(symbol->ir_name), "%%t%d",
        ctx->next_temp_id++);
    return symbol;
}

static const ParserNode *codegen_find_param(const FunctionContext *ctx,
    Token name)
{
    const ParserNode *param = ctx->params;
    size_t index = 0;

    for (index = 0; index < ctx->param_count; index++) {
        if (param && codegen_name_matches(name, param->token.start,
                param->token.length)) {
            return param;
        }

        if (param) {
            param = param->next;
        }
    }

    return NULL;
}

static int codegen_set_error(Codegen *codegen, const char *message)
{
    if (!codegen->error_message) {
        codegen->error_message = message;
    }

    return 0;
}

static int codegen_function_parts(Codegen *codegen, const ParserNode *node,
    const ParserNode **param_list_out,
    size_t *param_count_out,
    const ParserNode **body_out)
{
    const ParserNode *child = NULL;
    const ParserNode *param_list = NULL;
    const ParserNode *body = NULL;
    size_t param_count = 0;

    if (!node->first_child) {
        return codegen_set_error(codegen,
            "codegen: expected function body");
    }

    for (child = node->first_child; child; child = child->next) {
        if (!child->next) {
            body = child;
            break;
        }

        if (child->type != PARSER_NODE_DECLARATION) {
            return codegen_set_error(codegen,
                "codegen: expected parameter declaration");
        }

        if (param_count == 0) {
            param_list = child;
        }

        param_count++;
    }

    if (!body || body->type != PARSER_NODE_BLOCK) {
        return codegen_set_error(codegen,
            "codegen: expected function block");
    }

    *param_list_out = param_count ? param_list : NULL;
    *param_count_out = param_count;
    *body_out = body;
    return 1;
}

void codegen_init(Codegen *codegen, const char *input)
{
    codegen->input = input;
    codegen->error_message = NULL;
    checker_init(&codegen->checker, input);
    parser_init(&codegen->parser, input);
}

static int codegen_emit_declaration(Codegen *codegen, const ParserNode *node,
    const GlobalSymbol *globals, size_t global_count, FILE *out)
{
    long value = 0;
    char type_name[32];
    char init_value[64];

    if (node->type != PARSER_NODE_DECLARATION) {
        return codegen_set_error(codegen,
            "codegen: expected declaration node");
    }

    if (node->token.type != TOKEN_IDENT) {
        return codegen_set_error(codegen,
            "codegen: expected identifier token");
    }

    codegen_format_type(node->type_token, node->pointer_depth,
        type_name, sizeof(type_name));

    if (node->first_child) {
        const ParserNode *init = node->first_child;

        if (node->first_child->next) {
            return codegen_set_error(codegen,
                "codegen: unexpected initializer list");
        }

        if (node->pointer_depth == 0) {
            if (init->type != PARSER_NODE_NUMBER) {
                return codegen_set_error(codegen,
                    "codegen: expected number initializer");
            }

            value = init->token.value;
            snprintf(init_value, sizeof(init_value), "%ld", value);
        } else if (init->type == PARSER_NODE_NUMBER) {
            if (init->token.value != 0) {
                return codegen_set_error(codegen,
                    "codegen: expected null pointer initializer");
            }
            snprintf(init_value, sizeof(init_value), "null");
        } else if (init->type == PARSER_NODE_UNARY
            && token_is_punct(init->token, "&")) {
            const ParserNode *operand = init->first_child;
            const GlobalSymbol *symbol = NULL;

            if (!operand || operand->next) {
                return codegen_set_error(codegen,
                    "codegen: expected address-of operand");
            }

            if (operand->type != PARSER_NODE_IDENTIFIER) {
                return codegen_set_error(codegen,
                    "codegen: expected identifier address");
            }

            symbol = NULL;
            for (size_t i = 0; i < global_count; i++) {
                if (codegen_name_matches(operand->token,
                        globals[i].name,
                        globals[i].length)) {
                    symbol = &globals[i];
                    break;
                }
            }

            if (!symbol) {
                return codegen_set_error(codegen,
                    "codegen: unknown global initializer");
            }

            if (symbol->type_token.type != node->type_token.type
                || node->pointer_depth != symbol->pointer_depth + 1) {
                return codegen_set_error(codegen,
                    "codegen: initializer type mismatch");
            }

            snprintf(init_value, sizeof(init_value), "@%.*s",
                (int)operand->token.length,
                operand->token.start);
        } else {
            return codegen_set_error(codegen,
                "codegen: unsupported initializer");
        }
    } else if (node->pointer_depth > 0) {
        snprintf(init_value, sizeof(init_value), "null");
    } else {
        snprintf(init_value, sizeof(init_value), "0");
    }

    fprintf(out, "@%.*s = global %s %s\n",
        (int)node->token.length,
        node->token.start,
        type_name,
        init_value);

    return 1;
}

static int codegen_emit_expression(FunctionContext *ctx,
    const ParserNode *node,
    char *value,
    size_t value_size,
    TypeDesc *type_out);

static void codegen_format_label(char *buffer, size_t size, const char *prefix,
    int id);

static int codegen_emit_logical_binary(FunctionContext *ctx,
    const ParserNode *node,
    char *value,
    size_t value_size,
    int is_and,
    TypeDesc *type_out)
{
    const ParserNode *left = node->first_child;
    const ParserNode *right = left ? left->next : NULL;
    char left_value[32];
    char right_value[32];
    char left_bool[32];
    char right_bool[32];
    char result_bool[32];
    char result_value[32];
    char left_label[32];
    char rhs_label[32];
    char end_label[32];
    TypeDesc left_type;
    TypeDesc right_type;

    if (!left || !right || right->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected binary operands");
    }

    codegen_format_label(left_label, sizeof(left_label), "logic.left",
        ctx->next_label_id++);
    codegen_format_label(rhs_label, sizeof(rhs_label), "logic.rhs",
        ctx->next_label_id++);
    codegen_format_label(end_label, sizeof(end_label), "logic.end",
        ctx->next_label_id++);

    fprintf(ctx->out, "  br label %%%s\n", left_label);
    fprintf(ctx->out, "%s:\n", left_label);

    if (!codegen_emit_expression(ctx, left, left_value, sizeof(left_value),
        &left_type)) {
        return 0;
    }

    if (!codegen_type_is_integer(left_type)) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected integer logical operand");
    }

    snprintf(left_bool, sizeof(left_bool), "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out, "  %s = icmp ne i32 %s, 0\n", left_bool, left_value);

    if (is_and) {
        fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
            left_bool,
            rhs_label,
            end_label);
    } else {
        fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
            left_bool,
            end_label,
            rhs_label);
    }

    fprintf(ctx->out, "%s:\n", rhs_label);
    if (!codegen_emit_expression(ctx, right, right_value,
        sizeof(right_value), &right_type)) {
        return 0;
    }

    if (!codegen_type_is_integer(right_type)) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected integer logical operand");
    }

    snprintf(right_bool, sizeof(right_bool), "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out, "  %s = icmp ne i32 %s, 0\n", right_bool, right_value);
    fprintf(ctx->out, "  br label %%%s\n", end_label);

    fprintf(ctx->out, "%s:\n", end_label);
    snprintf(result_bool, sizeof(result_bool), "%%t%d", ctx->next_temp_id++);
    if (is_and) {
        fprintf(ctx->out, "  %s = phi i1 [0, %%%s], [%s, %%%s]\n",
            result_bool,
            left_label,
            right_bool,
            rhs_label);
    } else {
        fprintf(ctx->out, "  %s = phi i1 [1, %%%s], [%s, %%%s]\n",
            result_bool,
            left_label,
            right_bool,
            rhs_label);
    }

    snprintf(result_value, sizeof(result_value), "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out, "  %s = zext i1 %s to i32\n",
        result_value,
        result_bool);

    snprintf(value, value_size, "%s", result_value);
    *type_out = codegen_int_type_desc();
    return 1;
}

static int codegen_emit_expression(FunctionContext *ctx,
    const ParserNode *node,
    char *value,
    size_t value_size,
    TypeDesc *type_out)
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
        *type_out = codegen_int_type_desc();
        return 1;
    }

    if (node->type == PARSER_NODE_CALL) {
        const FunctionSymbol *symbol = NULL;
        const ParserNode *arg = NULL;
        const ParserNode *param = NULL;
        size_t arg_count = 0;
        size_t index = 0;
        char (*arg_values)[32] = NULL;
        char (*arg_types)[32] = NULL;
        char type_name[32];

        if (node->token.type != TOKEN_IDENT) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected call identifier");
        }

        symbol = codegen_find_function(ctx, node->token);
        if (!symbol) {
            return codegen_set_error(ctx->codegen,
                "codegen: unknown function");
        }

        arg_count = symbol->param_count;
        if (arg_count > 0) {
            arg_values = malloc(arg_count * sizeof(*arg_values));
            arg_types = malloc(arg_count * sizeof(*arg_types));
            if (!arg_values || !arg_types) {
                free(arg_values);
                free(arg_types);
                return codegen_set_error(ctx->codegen,
                    "codegen: out of memory");
            }
        }

        arg = node->first_child;
        param = symbol->param_list;
        for (index = 0; index < arg_count; index++) {
            TypeDesc arg_type;
            TypeDesc param_type;
            TypeInfo param_info;

            if (!arg || !param) {
                free(arg_values);
                free(arg_types);
                return codegen_set_error(ctx->codegen,
                    "codegen: argument count mismatch");
            }

            if (!codegen_emit_expression(ctx, arg,
                    arg_values[index],
                    sizeof(arg_values[index]),
                    &arg_type)) {
                free(arg_values);
                free(arg_types);
                return 0;
            }

            param_type = codegen_make_type_desc(param->type_token,
                param->pointer_depth);

            if (param_type.pointer_depth > 0) {
                if (arg_type.pointer_depth != param_type.pointer_depth
                    || arg_type.base_token != param_type.base_token) {
                    free(arg_values);
                    free(arg_types);
                    return codegen_set_error(ctx->codegen,
                        "codegen: argument type mismatch");
                }
            } else {
                if (!codegen_type_is_integer(arg_type)) {
                    free(arg_values);
                    free(arg_types);
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer argument");
                }
            }

            codegen_format_type(param->type_token, param->pointer_depth,
                arg_types[index], sizeof(arg_types[index]));

            param_info = codegen_type_info(param->type_token);
            if (param_type.pointer_depth == 0 && param_info.width < 32) {
                char cast_value[32];

                snprintf(cast_value, sizeof(cast_value), "%%t%d",
                    ctx->next_temp_id++);
                fprintf(ctx->out, "  %s = trunc i32 %s to %s\n",
                    cast_value,
                    arg_values[index],
                    arg_types[index]);
                snprintf(arg_values[index], sizeof(arg_values[index]), "%s",
                    cast_value);
            }

            arg = arg->next;
            param = param->next;
        }

        if (arg) {
            free(arg_values);
            free(arg_types);
            return codegen_set_error(ctx->codegen,
                "codegen: argument count mismatch");
        }

        codegen_format_type(symbol->type_token, symbol->pointer_depth,
            type_name, sizeof(type_name));

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = call %s @%.*s(",
            value,
            type_name,
            (int)node->token.length,
            node->token.start);
        for (index = 0; index < arg_count; index++) {
            if (index > 0) {
                fprintf(ctx->out, ", ");
            }
            fprintf(ctx->out, "%s %s", arg_types[index], arg_values[index]);
        }
        fprintf(ctx->out, ")\n");
        free(arg_values);
        free(arg_types);
        *type_out = codegen_make_type_desc(symbol->type_token,
            symbol->pointer_depth);
        return 1;
    }

    if (node->type == PARSER_NODE_IDENTIFIER) {
        const LocalSymbol *local = NULL;
        const ParserNode *param = NULL;
        const GlobalSymbol *symbol = NULL;
        char value_type[32];
        char pointer_type[32];

        if (node->token.type != TOKEN_IDENT) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected identifier");
        }

        if (node->first_child) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected identifier children");
        }

        local = codegen_find_local(ctx, node->token);
        if (local) {
            codegen_format_type(local->type_token, local->pointer_depth,
                value_type, sizeof(value_type));
            codegen_format_type(local->type_token, local->pointer_depth + 1,
                pointer_type, sizeof(pointer_type));

            snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = load %s, %s %s\n",
                value,
                value_type,
                pointer_type,
                local->ir_name);
            *type_out = codegen_make_type_desc(local->type_token,
                local->pointer_depth);
            return 1;
        }

        param = codegen_find_param(ctx, node->token);
        if (param) {
            snprintf(value, value_size, "%%%.*s",
                (int)param->token.length,
                param->token.start);
            *type_out = codegen_make_type_desc(param->type_token,
                param->pointer_depth);
            return 1;
        }

        symbol = codegen_find_global(ctx, node->token);
        if (!symbol) {
            return codegen_set_error(ctx->codegen,
                "codegen: unknown global");
        }

        codegen_format_type(symbol->type_token, symbol->pointer_depth,
            value_type, sizeof(value_type));
        codegen_format_type(symbol->type_token, symbol->pointer_depth + 1,
            pointer_type, sizeof(pointer_type));

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = load %s, %s @%.*s\n",
            value,
            value_type,
            pointer_type,
            (int)node->token.length,
            node->token.start);
        *type_out = codegen_make_type_desc(symbol->type_token,
            symbol->pointer_depth);
        return 1;
    }

    if (node->type == PARSER_NODE_UNARY) {
        const ParserNode *operand = node->first_child;
        char operand_value[32];
        char temp[32];
        char result[32];
        TypeDesc operand_type;

        if (!operand || operand->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected unary operand");
        }

        if (token_is_punct(node->token, "&")) {
            const GlobalSymbol *symbol = NULL;

            if (operand->type != PARSER_NODE_IDENTIFIER) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected identifier address");
            }

            symbol = codegen_find_global(ctx, operand->token);
            if (!symbol) {
                return codegen_set_error(ctx->codegen,
                    "codegen: unknown global");
            }

            snprintf(value, value_size, "@%.*s",
                (int)operand->token.length,
                operand->token.start);
            *type_out = codegen_make_type_desc(symbol->type_token,
                symbol->pointer_depth + 1);
            return 1;
        }

        if (!codegen_emit_expression(ctx, operand, operand_value,
            sizeof(operand_value), &operand_type)) {
            return 0;
        }

        if (token_is_punct(node->token, "!")) {
            if (!codegen_type_is_integer(operand_type)) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected integer operand");
            }

            snprintf(temp, sizeof(temp), "%%t%d", ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = icmp eq i32 %s, 0\n", temp,
                operand_value);

            snprintf(result, sizeof(result), "%%t%d", ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = zext i1 %s to i32\n", result, temp);

            snprintf(value, value_size, "%s", result);
            *type_out = codegen_int_type_desc();
            return 1;
        }

        if (token_is_punct(node->token, "+")) {
            if (!codegen_type_is_integer(operand_type)) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected integer operand");
            }

            snprintf(value, value_size, "%s", operand_value);
            *type_out = operand_type;
            return 1;
        }

        if (token_is_punct(node->token, "-")) {
            if (!codegen_type_is_integer(operand_type)) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected integer operand");
            }

            snprintf(result, sizeof(result), "%%t%d", ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = sub i32 0, %s\n", result,
                operand_value);
            snprintf(value, value_size, "%s", result);
            *type_out = codegen_int_type_desc();
            return 1;
        }

        if (token_is_punct(node->token, "*")) {
            char load_type[32];
            char pointer_type[32];

            if (operand_type.pointer_depth <= 0) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected pointer operand");
            }

            codegen_format_desc_type(operand_type, pointer_type,
                sizeof(pointer_type));
            operand_type.pointer_depth--;
            codegen_format_desc_type(operand_type, load_type,
                sizeof(load_type));

            snprintf(result, sizeof(result), "%%t%d", ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = load %s, %s %s\n",
                result,
                load_type,
                pointer_type,
                operand_value);
            snprintf(value, value_size, "%s", result);
            *type_out = operand_type;
            return 1;
        }

        return codegen_set_error(ctx->codegen,
            "codegen: expected unary operator");
    }

    if (node->type == PARSER_NODE_BINARY) {
        const ParserNode *left = node->first_child;
        const ParserNode *right = left ? left->next : NULL;
        char left_value[32];
        char right_value[32];
        char pointer_value[32];
        char offset_value[32];
        char element_type_name[32];
        char pointer_type_name[32];
        TypeDesc pointer_type;
        TypeDesc element_type;
        const char *opcode = NULL;
        TypeDesc left_type;
        TypeDesc right_type;

        if (!left || !right || right->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected binary operands");
        }

        if (token_is_punct(node->token, "&&")) {
            return codegen_emit_logical_binary(ctx, node, value,
                value_size, 1, type_out);
        }

        if (token_is_punct(node->token, "||")) {
            return codegen_emit_logical_binary(ctx, node, value,
                value_size, 0, type_out);
        }

        if (!codegen_emit_expression(ctx, left, left_value,
            sizeof(left_value), &left_type)) {
            return 0;
        }

        if (!codegen_emit_expression(ctx, right, right_value,
            sizeof(right_value), &right_type)) {
            return 0;
        }

        if (token_is_punct(node->token, "+")) {
            if (left_type.pointer_depth > 0
                && codegen_type_is_integer(right_type)) {
                pointer_type = left_type;
                element_type = left_type;
                element_type.pointer_depth--;
                codegen_format_desc_type(element_type, element_type_name,
                    sizeof(element_type_name));
                codegen_format_desc_type(pointer_type, pointer_type_name,
                    sizeof(pointer_type_name));
                snprintf(pointer_value, sizeof(pointer_value), "%s",
                    left_value);
                snprintf(offset_value, sizeof(offset_value), "%s",
                    right_value);
                snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
                fprintf(ctx->out,
                    "  %s = getelementptr %s, %s %s, i32 %s\n",
                    value,
                    element_type_name,
                    pointer_type_name,
                    pointer_value,
                    offset_value);
                *type_out = pointer_type;
                return 1;
            }
            if (right_type.pointer_depth > 0
                && codegen_type_is_integer(left_type)) {
                pointer_type = right_type;
                element_type = right_type;
                element_type.pointer_depth--;
                codegen_format_desc_type(element_type, element_type_name,
                    sizeof(element_type_name));
                codegen_format_desc_type(pointer_type, pointer_type_name,
                    sizeof(pointer_type_name));
                snprintf(pointer_value, sizeof(pointer_value), "%s",
                    right_value);
                snprintf(offset_value, sizeof(offset_value), "%s",
                    left_value);
                snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
                fprintf(ctx->out,
                    "  %s = getelementptr %s, %s %s, i32 %s\n",
                    value,
                    element_type_name,
                    pointer_type_name,
                    pointer_value,
                    offset_value);
                *type_out = pointer_type;
                return 1;
            }
            opcode = "add";
        } else if (token_is_punct(node->token, "-")) {
            if (left_type.pointer_depth > 0
                && codegen_type_is_integer(right_type)) {
                char neg_value[32];

                pointer_type = left_type;
                element_type = left_type;
                element_type.pointer_depth--;
                codegen_format_desc_type(element_type, element_type_name,
                    sizeof(element_type_name));
                codegen_format_desc_type(pointer_type, pointer_type_name,
                    sizeof(pointer_type_name));
                snprintf(pointer_value, sizeof(pointer_value), "%s",
                    left_value);
                snprintf(neg_value, sizeof(neg_value), "%%t%d",
                    ctx->next_temp_id++);
                fprintf(ctx->out, "  %s = sub i32 0, %s\n",
                    neg_value, right_value);
                snprintf(offset_value, sizeof(offset_value), "%s",
                    neg_value);
                snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
                fprintf(ctx->out,
                    "  %s = getelementptr %s, %s %s, i32 %s\n",
                    value,
                    element_type_name,
                    pointer_type_name,
                    pointer_value,
                    offset_value);
                *type_out = pointer_type;
                return 1;
            }
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

        if (!codegen_type_is_integer(left_type)
            || !codegen_type_is_integer(right_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer operands");
        }

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = %s i32 %s, %s\n",
            value, opcode, left_value, right_value);
        *type_out = codegen_int_type_desc();
        return 1;
    }

    return codegen_set_error(ctx->codegen,
        "codegen: expected expression");
}

static int codegen_emit_local_declaration(FunctionContext *ctx,
    const ParserNode *node)
{
    LocalSymbol *local = NULL;
    char type_name[32];
    char init_value[32];
    TypeDesc init_type;

    if (node->type != PARSER_NODE_DECLARATION) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected declaration");
    }

    if (node->token.type != TOKEN_IDENT) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected identifier token");
    }

    local = codegen_add_local(ctx, node);
    if (!local) {
        return 0;
    }

    codegen_format_type(node->type_token, node->pointer_depth,
        type_name, sizeof(type_name));
    fprintf(ctx->out, "  %s = alloca %s\n", local->ir_name, type_name);

    if (!node->first_child) {
        return 1;
    }

    if (node->first_child->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected initializer list");
    }

    if (!codegen_emit_expression(ctx, node->first_child, init_value,
        sizeof(init_value), &init_type)) {
        return 0;
    }

    if (node->pointer_depth > 0) {
        if (init_type.pointer_depth != node->pointer_depth
            || init_type.base_token != node->type_token.type) {
            return codegen_set_error(ctx->codegen,
                "codegen: initializer type mismatch");
        }
    } else {
        TypeInfo init_info = codegen_type_info(node->type_token);
        if (!codegen_type_is_integer(init_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer initializer");
        }

        if (init_info.width < 32) {
            char cast_value[32];

            snprintf(cast_value, sizeof(cast_value), "%%t%d",
                ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = trunc i32 %s to %s\n",
                cast_value,
                init_value,
                type_name);
            snprintf(init_value, sizeof(init_value), "%s", cast_value);
        }
    }

    fprintf(ctx->out, "  store %s %s, %s* %s\n",
        type_name,
        init_value,
        type_name,
        local->ir_name);
    return 1;
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
    TypeDesc condition_type;

    if (!condition || !then_branch) {
        return codegen_set_error(ctx->codegen,
            "codegen: incomplete if statement");
    }

    if (else_branch && else_branch->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected else statement");
    }

    if (!codegen_emit_expression(ctx, condition, value, sizeof(value),
        &condition_type)) {
        return 0;
    }

    if (!codegen_type_is_integer(condition_type)) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected integer condition");
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
    TypeDesc condition_type;

    if (!condition || !body) {
        return codegen_set_error(ctx->codegen,
            "codegen: incomplete while statement");
    }

    if (body->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected while statement");
    }

    codegen_format_label(cond_label, sizeof(cond_label), "while.cond",
        ctx->next_label_id++);
    codegen_format_label(body_label, sizeof(body_label), "while.body",
        ctx->next_label_id++);
    codegen_format_label(end_label, sizeof(end_label), "while.end",
        ctx->next_label_id++);

    fprintf(ctx->out, "  br label %%%s\n", cond_label);
    fprintf(ctx->out, "%s:\n", cond_label);

    if (!codegen_emit_expression(ctx, condition, value, sizeof(value),
        &condition_type)) {
        return 0;
    }

    if (!codegen_type_is_integer(condition_type)) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected integer condition");
    }

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

static int codegen_emit_for(FunctionContext *ctx, const ParserNode *node)
{
    const ParserNode *init = node->first_child;
    const ParserNode *condition = init ? init->next : NULL;
    const ParserNode *increment = condition ? condition->next : NULL;
    const ParserNode *body = increment ? increment->next : NULL;
    char value[32];
    char temp[32];
    char cond_label[32];
    char body_label[32];
    char inc_label[32];
    char end_label[32];
    int body_terminated = 0;
    TypeDesc condition_type;

    if (!init || !condition || !increment || !body) {
        return codegen_set_error(ctx->codegen,
            "codegen: incomplete for statement");
    }

    if (body->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected for statement");
    }

    if (codegen_emit_statement(ctx, init)) {
        return 1;
    }

    if (ctx->codegen->error_message) {
        return 0;
    }

    codegen_format_label(cond_label, sizeof(cond_label), "for.cond",
        ctx->next_label_id++);
    codegen_format_label(body_label, sizeof(body_label), "for.body",
        ctx->next_label_id++);
    codegen_format_label(inc_label, sizeof(inc_label), "for.inc",
        ctx->next_label_id++);
    codegen_format_label(end_label, sizeof(end_label), "for.end",
        ctx->next_label_id++);

    fprintf(ctx->out, "  br label %%%s\n", cond_label);
    fprintf(ctx->out, "%s:\n", cond_label);

    if (condition->type == PARSER_NODE_EMPTY) {
        fprintf(ctx->out, "  br label %%%s\n", body_label);
    } else {
        if (!codegen_emit_expression(ctx, condition, value, sizeof(value),
            &condition_type)) {
            return 0;
        }

        if (!codegen_type_is_integer(condition_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer condition");
        }

        snprintf(temp, sizeof(temp), "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = icmp ne i32 %s, 0\n", temp, value);
        fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
            temp,
            body_label,
            end_label);
    }

    fprintf(ctx->out, "%s:\n", body_label);
    body_terminated = codegen_emit_statement(ctx, body);
    if (ctx->codegen->error_message) {
        return 0;
    }
    if (!body_terminated) {
        fprintf(ctx->out, "  br label %%%s\n", inc_label);
    }

    fprintf(ctx->out, "%s:\n", inc_label);
    if (increment->type != PARSER_NODE_EMPTY) {
        codegen_emit_statement(ctx, increment);
        if (ctx->codegen->error_message) {
            return 0;
        }
    }

    fprintf(ctx->out, "  br label %%%s\n", cond_label);
    fprintf(ctx->out, "%s:\n", end_label);
    return 0;
}

static int codegen_emit_statement(FunctionContext *ctx, const ParserNode *node)
{
    char value[32];
    char cast_value[32];
    TypeDesc expr_type;

    switch (node->type) {
    case PARSER_NODE_BLOCK:
        return codegen_emit_block(ctx, node);
    case PARSER_NODE_DECLARATION:
        if (!codegen_emit_local_declaration(ctx, node)) {
            return 0;
        }
        return 0;
    case PARSER_NODE_IF:
        return codegen_emit_if(ctx, node);
    case PARSER_NODE_ASSIGN: {
        const LocalSymbol *local = NULL;
        const GlobalSymbol *global = NULL;
        const ParserNode *param = NULL;
        const ParserNode *left = node->first_child;
        const ParserNode *right = left ? left->next : NULL;
        char value[32];
        char type_name[32];
        TypeDesc expr_type;

        if (!left || !right || right->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected assignment expression");
        }

        if (left->type == PARSER_NODE_UNARY
            && token_is_punct(left->token, "*")) {
            const ParserNode *operand = left->first_child;
            char pointer_value[32];
            TypeDesc pointer_type;
            TypeDesc target_type;
            TypeInfo info;
            Token target_token;

            if (!operand || operand->next) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected assignment target");
            }

            if (!codegen_emit_expression(ctx, operand, pointer_value,
                sizeof(pointer_value), &pointer_type)) {
                return 0;
            }

            if (pointer_type.pointer_depth <= 0) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected pointer assignment");
            }

            if (!codegen_emit_expression(ctx, right, value,
                sizeof(value), &expr_type)) {
                return 0;
            }

            target_type = pointer_type;
            target_type.pointer_depth--;
            codegen_format_desc_type(target_type, type_name,
                sizeof(type_name));

            if (target_type.pointer_depth > 0) {
                if (expr_type.pointer_depth != target_type.pointer_depth
                    || expr_type.base_token != target_type.base_token) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: assignment type mismatch");
                }
            } else {
                if (!codegen_type_is_integer(expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer assignment");
                }

                target_token.type = target_type.base_token;
                target_token.start = NULL;
                target_token.length = 0;
                target_token.value = 0;
                info = codegen_type_info(target_token);
                if (info.width < 32) {
                    char cast_value[32];

                    snprintf(cast_value, sizeof(cast_value), "%%t%d",
                        ctx->next_temp_id++);
                    fprintf(ctx->out, "  %s = trunc i32 %s to %s\n",
                        cast_value,
                        value,
                        type_name);
                    snprintf(value, sizeof(value), "%s", cast_value);
                }
            }

            fprintf(ctx->out, "  store %s %s, %s* %s\n",
                type_name,
                value,
                type_name,
                pointer_value);
            return 0;
        }

        if (left->type != PARSER_NODE_IDENTIFIER) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected assignment target");
        }

        if (!codegen_emit_expression(ctx, right, value,
            sizeof(value), &expr_type)) {
            return 0;
        }

        local = codegen_find_local(ctx, left->token);
        if (local) {
            codegen_format_type(local->type_token, local->pointer_depth,
                type_name, sizeof(type_name));

            if (local->pointer_depth > 0) {
                if (expr_type.pointer_depth != local->pointer_depth
                    || expr_type.base_token != local->type_token.type) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: assignment type mismatch");
                }
            } else {
                TypeInfo info = codegen_type_info(local->type_token);
                if (!codegen_type_is_integer(expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer assignment");
                }

                if (info.width < 32) {
                    char cast_value[32];

                    snprintf(cast_value, sizeof(cast_value), "%%t%d",
                        ctx->next_temp_id++);
                    fprintf(ctx->out, "  %s = trunc i32 %s to %s\n",
                        cast_value,
                        value,
                        type_name);
                    snprintf(value, sizeof(value), "%s", cast_value);
                }
            }

            fprintf(ctx->out, "  store %s %s, %s* %s\n",
                type_name,
                value,
                type_name,
                local->ir_name);
            return 0;
        }

        param = codegen_find_param(ctx, left->token);
        if (param) {
            return codegen_set_error(ctx->codegen,
                "codegen: assignment to parameter not supported");
        }

        global = codegen_find_global(ctx, left->token);
        if (!global) {
            return codegen_set_error(ctx->codegen,
                "codegen: unknown assignment target");
        }

        codegen_format_type(global->type_token, global->pointer_depth,
            type_name, sizeof(type_name));

        if (global->pointer_depth > 0) {
            if (expr_type.pointer_depth != global->pointer_depth
                || expr_type.base_token != global->type_token.type) {
                return codegen_set_error(ctx->codegen,
                    "codegen: assignment type mismatch");
            }
        } else {
            TypeInfo info = codegen_type_info(global->type_token);
            if (!codegen_type_is_integer(expr_type)) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected integer assignment");
            }

            if (info.width < 32) {
                char cast_value[32];

                snprintf(cast_value, sizeof(cast_value), "%%t%d",
                    ctx->next_temp_id++);
                fprintf(ctx->out, "  %s = trunc i32 %s to %s\n",
                    cast_value,
                    value,
                    type_name);
                snprintf(value, sizeof(value), "%s", cast_value);
            }
        }

        fprintf(ctx->out, "  store %s %s, %s* @%.*s\n",
            type_name,
            value,
            type_name,
            (int)left->token.length,
            left->token.start);
        return 0;
    }
    case PARSER_NODE_WHILE:
        return codegen_emit_while(ctx, node);
    case PARSER_NODE_FOR:
        return codegen_emit_for(ctx, node);
    case PARSER_NODE_RETURN:
        if (!node->first_child || node->first_child->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected return statement");
        }

        if (!codegen_emit_expression(ctx, node->first_child, value,
            sizeof(value), &expr_type)) {
            return 0;
        }

        if (ctx->return_pointer_depth > 0) {
            if (expr_type.pointer_depth != ctx->return_pointer_depth
                || expr_type.base_token != ctx->return_base_token) {
                return codegen_set_error(ctx->codegen,
                    "codegen: return type mismatch");
            }

            fprintf(ctx->out, "  ret %s %s\n", ctx->return_type, value);
            return 1;
        }

        if (!codegen_type_is_integer(expr_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer return");
        }

        if (ctx->return_width == 32) {
            fprintf(ctx->out, "  ret i32 %s\n", value);
            return 1;
        }

        snprintf(cast_value, sizeof(cast_value), "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = trunc i32 %s to %s\n",
            cast_value,
            value,
            ctx->return_type);
        fprintf(ctx->out, "  ret %s %s\n", ctx->return_type, cast_value);
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
    const GlobalSymbol *globals, size_t global_count,
    const FunctionSymbol *functions, size_t function_count,
    FILE *out)
{
    FunctionContext ctx;
    int terminated = 0;
    TypeInfo type_info;
    const ParserNode *param_list = NULL;
    const ParserNode *body = NULL;
    const ParserNode *param = NULL;
    size_t param_count = 0;
    size_t index = 0;
    char param_type[32];

    if (node->type != PARSER_NODE_FUNCTION) {
        return codegen_set_error(codegen,
            "codegen: expected function");
    }

    if (node->token.type != TOKEN_IDENT) {
        return codegen_set_error(codegen,
            "codegen: expected identifier token");
    }

    if (!codegen_function_parts(codegen, node, &param_list, &param_count,
            &body)) {
        return 0;
    }

    ctx.codegen = codegen;
    ctx.out = out;
    ctx.next_label_id = 0;
    ctx.next_temp_id = 0;
    type_info = codegen_type_info(node->type_token);
    codegen_format_type(node->type_token, node->pointer_depth,
        ctx.return_type, sizeof(ctx.return_type));
    ctx.return_width = type_info.width;
    ctx.return_pointer_depth = node->pointer_depth;
    ctx.return_base_token = node->type_token.type;
    ctx.globals = globals;
    ctx.global_count = global_count;
    ctx.params = param_list;
    ctx.param_count = param_count;
    ctx.locals = NULL;
    ctx.local_count = 0;
    ctx.local_capacity = 0;
    ctx.functions = functions;
    ctx.function_count = function_count;

    fprintf(out, "define %s @%.*s(",
        ctx.return_type,
        (int)node->token.length,
        node->token.start);
    param = param_list;
    for (index = 0; index < param_count; index++) {
        if (index > 0) {
            fprintf(out, ", ");
        }

        if (!param) {
            return codegen_set_error(codegen,
                "codegen: expected parameter");
        }

        codegen_format_type(param->type_token, param->pointer_depth,
            param_type, sizeof(param_type));
        fprintf(out, "%s %%%.*s",
            param_type,
            (int)param->token.length,
            param->token.start);
        param = param->next;
    }
    fprintf(out, ") {\n");
    fprintf(out, "entry:\n");

    terminated = codegen_emit_block(&ctx, body);
    if (codegen->error_message) {
        return 0;
    }

    if (!terminated) {
        fprintf(out, "  ret %s 0\n", ctx.return_type);
    }

    fprintf(out, "}\n");
    free(ctx.locals);
    return 1;
}

static int codegen_emit_translation_unit(Codegen *codegen,
    const ParserNode *node,
    FILE *out)
{
    const ParserNode *child = NULL;
    GlobalSymbol *globals = NULL;
    FunctionSymbol *functions = NULL;
    size_t global_count = 0;
    size_t function_count = 0;
    size_t global_index = 0;
    size_t function_index = 0;
    int result = 0;

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
            global_count++;
            continue;
        }

        if (child->type == PARSER_NODE_FUNCTION) {
            function_count++;
            continue;
        }
    }

    if (global_count > 0) {
        globals = malloc(global_count * sizeof(*globals));
        if (!globals) {
            return codegen_set_error(codegen,
                "codegen: out of memory");
        }
    }

    if (function_count > 0) {
        functions = malloc(function_count * sizeof(*functions));
        if (!functions) {
            free(globals);
            return codegen_set_error(codegen,
                "codegen: out of memory");
        }
    }

    for (child = node->first_child; child; child = child->next) {
        if (child->type == PARSER_NODE_DECLARATION) {
            globals[global_index].name = child->token.start;
            globals[global_index].length = child->token.length;
            globals[global_index].type_token = child->type_token;
            globals[global_index].pointer_depth = child->pointer_depth;
            global_index++;
            continue;
        }

        if (child->type == PARSER_NODE_FUNCTION) {
            const ParserNode *param_list = NULL;
            const ParserNode *body = NULL;
            size_t param_count = 0;

            if (!codegen_function_parts(codegen, child, &param_list,
                    &param_count, &body)) {
                goto cleanup;
            }

            functions[function_index].name = child->token.start;
            functions[function_index].length = child->token.length;
            functions[function_index].type_token = child->type_token;
            functions[function_index].pointer_depth = child->pointer_depth;
            functions[function_index].param_list = param_list;
            functions[function_index].param_count = param_count;
            function_index++;
            continue;
        }
    }

    for (child = node->first_child; child; child = child->next) {
        if (child->type == PARSER_NODE_DECLARATION) {
            if (!codegen_emit_declaration(codegen, child, globals,
                global_count, out)) {
                goto cleanup;
            }
            continue;
        }

        if (child->type == PARSER_NODE_FUNCTION) {
            if (!codegen_emit_function(codegen, child, globals, global_count,
                functions, function_count, out)) {
                goto cleanup;
            }
            continue;
        }

        codegen_set_error(codegen, "codegen: unexpected top-level node");
        goto cleanup;
    }

    result = 1;

cleanup:
    free(globals);
    free(functions);
    return result;
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
