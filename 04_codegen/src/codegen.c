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
    Token return_type_token;
    int return_is_const;
    const struct GlobalSymbol *globals;
    size_t global_count;
    const struct StructSymbol *structs;
    size_t struct_count;
    struct TypedefSymbol *typedefs;
    size_t typedef_count;
    size_t typedef_capacity;
    int typedef_depth;
    const ParserNode *params;
    size_t param_count;
    struct LocalSymbol *locals;
    size_t local_count;
    size_t local_capacity;
    const struct FunctionSymbol *functions;
    size_t function_count;
    struct LoopContext *loop_stack;
    size_t loop_depth;
    size_t loop_capacity;
} FunctionContext;

typedef struct LoopContext {
    char break_label[32];
    char continue_label[32];
} LoopContext;

typedef struct TypeInfo {
    const char *ir_name;
    int width;
} TypeInfo;

typedef struct TypeDesc {
    Token type_token;
    int pointer_depth;
    int is_const;
} TypeDesc;

typedef struct StructSymbol {
    const char *name;
    size_t length;
    const ParserNode *fields;
    size_t field_count;
} StructSymbol;

typedef struct GlobalSymbol {
    const char *name;
    size_t length;
    Token type_token;
    int pointer_depth;
    int is_const;
} GlobalSymbol;

typedef struct FunctionSymbol {
    const char *name;
    size_t length;
    Token type_token;
    int pointer_depth;
    int is_const;
    const ParserNode *param_list;
    size_t param_count;
} FunctionSymbol;

typedef struct LocalSymbol {
    const char *name;
    size_t length;
    Token type_token;
    int pointer_depth;
    int is_const;
    char ir_name[32];
} LocalSymbol;

typedef struct TypedefSymbol {
    const char *name;
    size_t length;
    TypeDesc type;
    int scope_depth;
} TypedefSymbol;

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
    } else if (token.type == TOKEN_VOID) {
        info.ir_name = "i8";
        info.width = 8;
    }

    return info;
}

static TypeDesc codegen_make_type_desc(Token token, int pointer_depth,
    int is_const)
{
    TypeDesc desc;

    desc.type_token = token;
    desc.pointer_depth = pointer_depth;
    desc.is_const = is_const;
    return desc;
}

static TypeDesc codegen_int_type_desc(void)
{
    TypeDesc desc;
    Token token;

    token.type = TOKEN_INT;
    token.start = NULL;
    token.length = 0;
    token.value = 0;

    desc.type_token = token;
    desc.pointer_depth = 0;
    desc.is_const = 0;
    return desc;
}

static void codegen_format_type(Token token, int pointer_depth,
    char *buffer,
    size_t buffer_size)
{
    size_t length = 0;
    int i = 0;

    if (token.type == TOKEN_STRUCT) {
        snprintf(buffer, buffer_size, "%%struct.%.*s",
            (int)token.length, token.start);
    } else {
        TypeInfo info = codegen_type_info(token);

        snprintf(buffer, buffer_size, "%s", info.ir_name);
    }
    length = strlen(buffer);

    for (i = 0; i < pointer_depth && length + 1 < buffer_size; i++) {
        buffer[length++] = '*';
        buffer[length] = '\0';
    }
}

static void codegen_format_desc_type(TypeDesc desc, char *buffer,
    size_t buffer_size)
{
    codegen_format_type(desc.type_token, desc.pointer_depth, buffer,
        buffer_size);
}

static int codegen_type_is_integer(TypeDesc desc)
{
    return desc.pointer_depth == 0
        && desc.type_token.type != TOKEN_STRUCT
        && desc.type_token.type != TOKEN_VOID;
}

static int codegen_integer_width(TypeDesc desc)
{
    TypeInfo info;

    if (!codegen_type_is_integer(desc)) {
        return 0;
    }

    info = codegen_type_info(desc.type_token);
    return info.width;
}

static int codegen_emit_condition_bool(FunctionContext *ctx,
    TypeDesc condition_type,
    const char *condition_value,
    char *bool_value,
    size_t bool_size)
{
    if (codegen_type_is_integer(condition_type)) {
        snprintf(bool_value, bool_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = icmp ne i32 %s, 0\n",
            bool_value,
            condition_value);
        return 1;
    }

    if (condition_type.pointer_depth > 0) {
        char type_name[32];

        codegen_format_desc_type(condition_type, type_name, sizeof(type_name));
        snprintf(bool_value, bool_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = icmp ne %s %s, null\n",
            bool_value,
            type_name,
            condition_value);
        return 1;
    }

    return codegen_set_error(ctx->codegen,
        "codegen: expected condition value");
}

static int codegen_is_null_pointer_literal(const ParserNode *node)
{
    return node->type == PARSER_NODE_NUMBER
        && node->token.type == TOKEN_NUMBER
        && node->token.value == 0;
}

static int codegen_emit_integer_cast(FunctionContext *ctx,
    TypeDesc from,
    TypeDesc to,
    char *value,
    size_t value_size)
{
    int from_width = codegen_integer_width(from);
    int to_width = codegen_integer_width(to);
    char from_type[32];
    char to_type[32];
    char cast_value[32];

    if (from_width == 0 || to_width == 0) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected integer cast");
    }

    if (from_width == to_width) {
        return 1;
    }

    codegen_format_desc_type(from, from_type, sizeof(from_type));
    codegen_format_desc_type(to, to_type, sizeof(to_type));
    snprintf(cast_value, sizeof(cast_value), "%%t%d", ctx->next_temp_id++);

    if (from_width > to_width) {
        fprintf(ctx->out, "  %s = trunc %s %s to %s\n",
            cast_value,
            from_type,
            value,
            to_type);
    } else {
        fprintf(ctx->out, "  %s = sext %s %s to %s\n",
            cast_value,
            from_type,
            value,
            to_type);
    }

    snprintf(value, value_size, "%s", cast_value);
    return 1;
}

static int codegen_type_token_equals(Token left, Token right);
static int codegen_resolve_desc(Codegen *codegen,
    const TypedefSymbol *typedefs,
    size_t typedef_count,
    TypeDesc desc,
    TypeDesc *resolved);
static int codegen_require_type(Codegen *codegen,
    const StructSymbol *structs,
    size_t struct_count,
    const TypedefSymbol *typedefs,
    size_t typedef_count,
    TypeDesc desc,
    TypeDesc *resolved_out);
static const StructSymbol *codegen_find_struct(const StructSymbol *structs,
    size_t struct_count,
    Token name_token);
static const GlobalSymbol *codegen_find_global(const FunctionContext *ctx,
    Token name);
static const LocalSymbol *codegen_find_local(const FunctionContext *ctx,
    Token name);
static int codegen_expression_type(FunctionContext *ctx,
    const ParserNode *node,
    TypeDesc *type_out);
static int codegen_resolve_member_type(FunctionContext *ctx,
    const ParserNode *node,
    TypeDesc *field_type_out);
static int codegen_emit_sizeof_type(FunctionContext *ctx,
    TypeDesc target_type,
    char *value,
    size_t value_size);
static int codegen_emit_expression(FunctionContext *ctx,
    const ParserNode *node,
    char *value,
    size_t value_size,
    TypeDesc *type_out);
static int codegen_emit_member_pointer(FunctionContext *ctx,
    const ParserNode *node,
    char *pointer_value,
    size_t pointer_size,
    TypeDesc *field_type_out);

static int codegen_emit_struct_definition(Codegen *codegen,
    const StructSymbol *symbol,
    const StructSymbol *structs,
    size_t struct_count,
    const TypedefSymbol *typedefs,
    size_t typedef_count,
    FILE *out)
{
    const ParserNode *field = NULL;
    int first = 1;
    Token self_token;

    self_token.type = TOKEN_STRUCT;
    self_token.start = symbol->name;
    self_token.length = symbol->length;
    self_token.value = 0;

    fprintf(out, "%%struct.%.*s = type { ",
        (int)symbol->length,
        symbol->name);

    for (field = symbol->fields; field; field = field->next) {
        char field_type[32];
        TypeDesc field_desc;
        TypeDesc resolved_desc;

        field_desc = codegen_make_type_desc(field->type_token,
            field->pointer_depth, field->is_const);
        if (!codegen_require_type(codegen, structs, struct_count, typedefs,
                typedef_count, field_desc, &resolved_desc)) {
            return 0;
        }

        if (resolved_desc.pointer_depth == 0
            && resolved_desc.type_token.type == TOKEN_STRUCT
            && codegen_type_token_equals(resolved_desc.type_token,
                self_token)) {
            return codegen_set_error(codegen,
                "codegen: recursive struct field not supported");
        }

        codegen_format_desc_type(resolved_desc, field_type,
            sizeof(field_type));

        if (!first) {
            fprintf(out, ", ");
        }
        fprintf(out, "%s", field_type);
        first = 0;
    }

    fprintf(out, " }\n");
    return 1;
}

static int codegen_name_matches(Token token, const char *name, size_t length)
{
    if (token.length != length) {
        return 0;
    }

    return strncmp(token.start, name, length) == 0;
}

static int codegen_type_token_equals(Token left, Token right)
{
    if (left.type != right.type) {
        return 0;
    }

    if (left.type != TOKEN_STRUCT) {
        return 1;
    }

    if (left.length != right.length) {
        return 0;
    }

    return strncmp(left.start, right.start, left.length) == 0;
}

static const TypedefSymbol *codegen_find_typedef(const TypedefSymbol *typedefs,
    size_t typedef_count,
    Token name_token)
{
    size_t index = 0;

    for (index = 0; index < typedef_count; index++) {
        if (codegen_name_matches(name_token, typedefs[index].name,
                typedefs[index].length)) {
            return &typedefs[index];
        }
    }

    return NULL;
}

static const StructSymbol *codegen_find_struct(const StructSymbol *structs,
    size_t struct_count,
    Token name_token)
{
    size_t index = 0;

    for (index = 0; index < struct_count; index++) {
        if (codegen_name_matches(name_token, structs[index].name,
                structs[index].length)) {
            return &structs[index];
        }
    }

    return NULL;
}

static int codegen_resolve_desc(Codegen *codegen,
    const TypedefSymbol *typedefs,
    size_t typedef_count,
    TypeDesc desc,
    TypeDesc *resolved)
{
    TypeDesc current = desc;
    int limit = 32;

    while (current.type_token.type == TOKEN_IDENT) {
        const TypedefSymbol *symbol = codegen_find_typedef(typedefs,
            typedef_count, current.type_token);

        if (!symbol) {
            return codegen_set_error(codegen, "codegen: unknown typedef");
        }

        current.type_token = symbol->type.type_token;
        current.pointer_depth += symbol->type.pointer_depth;
        current.is_const = current.is_const || symbol->type.is_const;

        if (limit-- <= 0) {
            return codegen_set_error(codegen, "codegen: typedef cycle");
        }
    }

    *resolved = current;
    return 1;
}

static int codegen_require_type(Codegen *codegen,
    const StructSymbol *structs,
    size_t struct_count,
    const TypedefSymbol *typedefs,
    size_t typedef_count,
    TypeDesc desc,
    TypeDesc *resolved_out)
{
    TypeDesc resolved;

    if (!codegen_resolve_desc(codegen, typedefs, typedef_count, desc,
            &resolved)) {
        return 0;
    }

    if (resolved.type_token.type == TOKEN_VOID
        && resolved.pointer_depth == 0) {
        return codegen_set_error(codegen, "codegen: void type not supported");
    }

    if (resolved.type_token.type == TOKEN_STRUCT
        && !codegen_find_struct(structs, struct_count, resolved.type_token)) {
        return codegen_set_error(codegen, "codegen: unknown struct type");
    }

    if (resolved_out) {
        *resolved_out = resolved;
    }
    return 1;
}

static int codegen_pointer_compatible(TypeDesc target, TypeDesc value)
{
    if (target.pointer_depth != value.pointer_depth) {
        return 0;
    }

    if (target.type_token.type == TOKEN_VOID
        || value.type_token.type == TOKEN_VOID) {
        return value.is_const ? target.is_const : 1;
    }

    if (!codegen_type_token_equals(target.type_token, value.type_token)) {
        return 0;
    }

    if (value.is_const && !target.is_const) {
        return 0;
    }

    return 1;
}

static int codegen_emit_member_pointer(FunctionContext *ctx,
    const ParserNode *node,
    char *pointer_value,
    size_t pointer_size,
    TypeDesc *field_type_out)
{
    const ParserNode *base = node->first_child;
    const ParserNode *field = base ? base->next : NULL;
    const StructSymbol *symbol = NULL;
    Token struct_token;
    size_t field_index = 0;
    int found = 0;
    char base_value[32];
    char struct_type[32];
    int base_is_const = 0;

    if (!base || !field || field->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected member access operands");
    }

    if (field->type != PARSER_NODE_IDENTIFIER
        || field->token.type != TOKEN_IDENT) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected member field identifier");
    }

    if (token_is_punct(node->token, ".")) {
        const LocalSymbol *local = codegen_find_local(ctx, base->token);
        const GlobalSymbol *global = NULL;
        TypeDesc base_desc;
        TypeDesc resolved_desc;

        if (base->type != PARSER_NODE_IDENTIFIER) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected struct identifier");
        }

        if (local) {
            base_desc = codegen_make_type_desc(local->type_token,
                local->pointer_depth, local->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    base_desc, &resolved_desc)) {
                return 0;
            }
            if (resolved_desc.pointer_depth != 0
                || resolved_desc.type_token.type != TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected struct value");
            }
            snprintf(base_value, sizeof(base_value), "%s", local->ir_name);
            struct_token = resolved_desc.type_token;
            base_is_const = resolved_desc.is_const;
        } else {
            global = codegen_find_global(ctx, base->token);
            if (!global) {
                return codegen_set_error(ctx->codegen,
                    "codegen: unknown struct identifier");
            }

            base_desc = codegen_make_type_desc(global->type_token,
                global->pointer_depth, global->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    base_desc, &resolved_desc)) {
                return 0;
            }
            if (resolved_desc.pointer_depth != 0
                || resolved_desc.type_token.type != TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected struct value");
            }
            snprintf(base_value, sizeof(base_value), "@%.*s",
                (int)base->token.length,
                base->token.start);
            struct_token = resolved_desc.type_token;
            base_is_const = resolved_desc.is_const;
        }
    } else if (token_is_punct(node->token, "->")) {
        TypeDesc base_type;
        TypeDesc resolved_desc;

        if (!codegen_emit_expression(ctx, base, base_value,
                sizeof(base_value), &base_type)) {
            return 0;
        }

        if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                ctx->typedef_count, base_type, &resolved_desc)) {
            return 0;
        }
        if (resolved_desc.pointer_depth != 1
            || resolved_desc.type_token.type != TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected struct pointer");
        }
        if (!codegen_require_type(ctx->codegen, ctx->structs,
                ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                resolved_desc, NULL)) {
            return 0;
        }
        struct_token = resolved_desc.type_token;
        base_is_const = resolved_desc.is_const;
    } else {
        return codegen_set_error(ctx->codegen,
            "codegen: expected member access operator");
    }

    symbol = codegen_find_struct(ctx->structs, ctx->struct_count,
        struct_token);
    if (!symbol) {
        return codegen_set_error(ctx->codegen,
            "codegen: unknown struct type");
    }

    for (const ParserNode *field_node = symbol->fields;
        field_node;
        field_node = field_node->next) {
        if (codegen_name_matches(field->token,
                field_node->token.start,
                field_node->token.length)) {
            TypeDesc field_desc;
            TypeDesc resolved_desc;

            field_desc = codegen_make_type_desc(field_node->type_token,
                field_node->pointer_depth, field_node->is_const);
            if (base_is_const) {
                field_desc.is_const = 1;
            }
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    field_desc, &resolved_desc)) {
                return 0;
            }
            *field_type_out = resolved_desc;
            found = 1;
            break;
        }
        field_index++;
    }

    if (!found) {
        return codegen_set_error(ctx->codegen,
            "codegen: unknown struct field");
    }

    codegen_format_type(struct_token, 0, struct_type, sizeof(struct_type));
    snprintf(pointer_value, pointer_size, "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out,
        "  %s = getelementptr inbounds %s, %s* %s, i32 0, i32 %zu\n",
        pointer_value,
        struct_type,
        struct_type,
        base_value,
        field_index);
    return 1;
}

static int codegen_resolve_member_type(FunctionContext *ctx,
    const ParserNode *node,
    TypeDesc *field_type_out)
{
    const ParserNode *base = node->first_child;
    const ParserNode *field = base ? base->next : NULL;
    const StructSymbol *symbol = NULL;
    Token struct_token;
    size_t field_index = 0;
    int found = 0;
    int base_is_const = 0;

    if (!base || !field || field->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected member access operands");
    }

    if (field->type != PARSER_NODE_IDENTIFIER
        || field->token.type != TOKEN_IDENT) {
        return codegen_set_error(ctx->codegen,
            "codegen: expected member field identifier");
    }

    if (token_is_punct(node->token, ".")) {
        const LocalSymbol *local = codegen_find_local(ctx, base->token);
        const GlobalSymbol *global = NULL;
        TypeDesc base_desc;
        TypeDesc resolved_desc;

        if (base->type != PARSER_NODE_IDENTIFIER) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected struct identifier");
        }

        if (local) {
            base_desc = codegen_make_type_desc(local->type_token,
                local->pointer_depth, local->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    base_desc, &resolved_desc)) {
                return 0;
            }
            if (resolved_desc.pointer_depth != 0
                || resolved_desc.type_token.type != TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected struct value");
            }
            struct_token = resolved_desc.type_token;
            base_is_const = resolved_desc.is_const;
        } else {
            global = codegen_find_global(ctx, base->token);
            if (!global) {
                return codegen_set_error(ctx->codegen,
                    "codegen: unknown struct identifier");
            }

            base_desc = codegen_make_type_desc(global->type_token,
                global->pointer_depth, global->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    base_desc, &resolved_desc)) {
                return 0;
            }
            if (resolved_desc.pointer_depth != 0
                || resolved_desc.type_token.type != TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected struct value");
            }
            struct_token = resolved_desc.type_token;
            base_is_const = resolved_desc.is_const;
        }
    } else if (token_is_punct(node->token, "->")) {
        TypeDesc base_type;
        TypeDesc resolved_desc;

        if (!codegen_expression_type(ctx, base, &base_type)) {
            return 0;
        }

        if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                ctx->typedef_count, base_type, &resolved_desc)) {
            return 0;
        }
        if (resolved_desc.pointer_depth != 1
            || resolved_desc.type_token.type != TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected struct pointer");
        }
        if (!codegen_require_type(ctx->codegen, ctx->structs,
                ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                resolved_desc, NULL)) {
            return 0;
        }
        struct_token = resolved_desc.type_token;
        base_is_const = resolved_desc.is_const;
    } else {
        return codegen_set_error(ctx->codegen,
            "codegen: expected member access operator");
    }

    symbol = codegen_find_struct(ctx->structs, ctx->struct_count,
        struct_token);
    if (!symbol) {
        return codegen_set_error(ctx->codegen,
            "codegen: unknown struct type");
    }

    for (const ParserNode *field_node = symbol->fields;
        field_node;
        field_node = field_node->next) {
        if (codegen_name_matches(field->token,
                field_node->token.start,
                field_node->token.length)) {
            TypeDesc field_desc;
            TypeDesc resolved_desc;

            field_desc = codegen_make_type_desc(field_node->type_token,
                field_node->pointer_depth, field_node->is_const);
            if (base_is_const) {
                field_desc.is_const = 1;
            }
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    field_desc, &resolved_desc)) {
                return 0;
            }
            *field_type_out = resolved_desc;
            found = 1;
            break;
        }
        field_index++;
    }

    if (!found) {
        return codegen_set_error(ctx->codegen,
            "codegen: unknown struct field");
    }

    return 1;
}

static int codegen_emit_sizeof_type(FunctionContext *ctx,
    TypeDesc target_type,
    char *value,
    size_t value_size)
{
    char element_type[32];
    char pointer_type[32];
    char gep_value[32];
    TypeDesc resolved_type;

    if (!codegen_require_type(ctx->codegen, ctx->structs, ctx->struct_count,
            ctx->typedefs, ctx->typedef_count, target_type, &resolved_type)) {
        return 0;
    }

    codegen_format_desc_type(resolved_type, element_type,
        sizeof(element_type));
    codegen_format_type(resolved_type.type_token,
        resolved_type.pointer_depth + 1,
        pointer_type, sizeof(pointer_type));

    snprintf(gep_value, sizeof(gep_value), "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out, "  %s = getelementptr %s, %s null, i32 1\n",
        gep_value,
        element_type,
        pointer_type);
    snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
    fprintf(ctx->out, "  %s = ptrtoint %s %s to i32\n",
        value,
        pointer_type,
        gep_value);
    return 1;
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
    symbol->is_const = node->is_const;
    snprintf(symbol->ir_name, sizeof(symbol->ir_name), "%%t%d",
        ctx->next_temp_id++);
    return symbol;
}

static int codegen_add_typedef(FunctionContext *ctx, const ParserNode *node)
{
    TypedefSymbol *entries = NULL;
    TypeDesc alias_desc;

    if (ctx->typedef_count == ctx->typedef_capacity) {
        size_t new_capacity = ctx->typedef_capacity ? ctx->typedef_capacity * 2 : 8;

        entries = realloc(ctx->typedefs, new_capacity * sizeof(*entries));
        if (!entries) {
            return codegen_set_error(ctx->codegen, "codegen: out of memory");
        }
        ctx->typedefs = entries;
        ctx->typedef_capacity = new_capacity;
    }

    alias_desc = codegen_make_type_desc(node->type_token, node->pointer_depth,
        node->is_const);
    if (!codegen_require_type(ctx->codegen, ctx->structs, ctx->struct_count,
            ctx->typedefs, ctx->typedef_count, alias_desc, NULL)) {
        return 0;
    }

    entries = ctx->typedefs;
    entries[ctx->typedef_count].name = node->token.start;
    entries[ctx->typedef_count].length = node->token.length;
    entries[ctx->typedef_count].type = alias_desc;
    entries[ctx->typedef_count].scope_depth = ctx->typedef_depth;
    ctx->typedef_count++;
    return 1;
}

static void codegen_push_typedef_scope(FunctionContext *ctx)
{
    ctx->typedef_depth++;
}

static void codegen_pop_typedef_scope(FunctionContext *ctx)
{
    while (ctx->typedef_count > 0) {
        TypedefSymbol *entry = &ctx->typedefs[ctx->typedef_count - 1];

        if (entry->scope_depth != ctx->typedef_depth) {
            break;
        }
        ctx->typedef_count--;
    }

    if (ctx->typedef_depth > 0) {
        ctx->typedef_depth--;
    }
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

static int codegen_push_loop(FunctionContext *ctx, const char *break_label,
    const char *continue_label)
{
    LoopContext *loop = NULL;
    size_t next_capacity = 0;

    if (ctx->loop_depth == ctx->loop_capacity) {
        next_capacity = ctx->loop_capacity ? ctx->loop_capacity * 2 : 4;
        loop = realloc(ctx->loop_stack,
            next_capacity * sizeof(*ctx->loop_stack));
        if (!loop) {
            return codegen_set_error(ctx->codegen, "codegen: out of memory");
        }

        ctx->loop_stack = loop;
        ctx->loop_capacity = next_capacity;
    }

    loop = &ctx->loop_stack[ctx->loop_depth];
    snprintf(loop->break_label, sizeof(loop->break_label), "%s", break_label);
    snprintf(loop->continue_label, sizeof(loop->continue_label), "%s",
        continue_label);
    ctx->loop_depth += 1;
    return 1;
}

static void codegen_pop_loop(FunctionContext *ctx)
{
    if (ctx->loop_depth > 0) {
        ctx->loop_depth -= 1;
    }
}

static const LoopContext *codegen_current_loop(const FunctionContext *ctx)
{
    if (ctx->loop_depth == 0) {
        return NULL;
    }

    return &ctx->loop_stack[ctx->loop_depth - 1];
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
    const GlobalSymbol *globals, size_t global_count,
    const StructSymbol *structs, size_t struct_count,
    const TypedefSymbol *typedefs, size_t typedef_count, FILE *out)
{
    long value = 0;
    char type_name[32];
    char init_value[64];
    TypeDesc declared_type;
    TypeDesc resolved_type;

    if (node->type != PARSER_NODE_DECLARATION) {
        return codegen_set_error(codegen,
            "codegen: expected declaration node");
    }

    if (node->token.type != TOKEN_IDENT) {
        return codegen_set_error(codegen,
            "codegen: expected identifier token");
    }

    declared_type = codegen_make_type_desc(node->type_token,
        node->pointer_depth, node->is_const);
    if (!codegen_require_type(codegen, structs, struct_count, typedefs,
            typedef_count, declared_type, &resolved_type)) {
        return 0;
    }

    codegen_format_desc_type(resolved_type, type_name, sizeof(type_name));

    if (node->first_child) {
        const ParserNode *init = node->first_child;

        if (node->first_child->next) {
            return codegen_set_error(codegen,
                "codegen: unexpected initializer list");
        }

        if (resolved_type.type_token.type == TOKEN_STRUCT
            && resolved_type.pointer_depth == 0) {
            return codegen_set_error(codegen,
                "codegen: struct initializer not supported");
        }

        if (resolved_type.pointer_depth == 0) {
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
            TypeDesc symbol_desc;
            TypeDesc resolved_symbol;

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

            symbol_desc = codegen_make_type_desc(symbol->type_token,
                symbol->pointer_depth, symbol->is_const);
            if (!codegen_resolve_desc(codegen, typedefs, typedef_count,
                    symbol_desc, &resolved_symbol)) {
                return 0;
            }
            resolved_symbol.pointer_depth += 1;
            if (!codegen_pointer_compatible(resolved_type, resolved_symbol)) {
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
    } else if (resolved_type.pointer_depth > 0) {
        snprintf(init_value, sizeof(init_value), "null");
    } else if (resolved_type.type_token.type == TOKEN_STRUCT) {
        snprintf(init_value, sizeof(init_value), "zeroinitializer");
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

    if (!codegen_emit_condition_bool(ctx, left_type, left_value, left_bool,
            sizeof(left_bool))) {
        return 0;
    }

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

    if (!codegen_emit_condition_bool(ctx, right_type, right_value, right_bool,
            sizeof(right_bool))) {
        return 0;
    }
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

static int codegen_expression_type(FunctionContext *ctx,
    const ParserNode *node,
    TypeDesc *type_out)
{
    if (node->type == PARSER_NODE_NUMBER) {
        *type_out = codegen_int_type_desc();
        return 1;
    }

    if (node->type == PARSER_NODE_CALL) {
        const FunctionSymbol *symbol = NULL;
        const ParserNode *arg = NULL;
        const ParserNode *param = NULL;
        size_t arg_count = 0;
        size_t index = 0;
        TypeDesc return_type;
        TypeDesc resolved_return;

        if (node->token.type != TOKEN_IDENT) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected call identifier");
        }

        symbol = codegen_find_function(ctx, node->token);
        if (!symbol) {
            return codegen_set_error(ctx->codegen,
                "codegen: unknown function");
        }

        return_type = codegen_make_type_desc(symbol->type_token,
            symbol->pointer_depth, symbol->is_const);
        if (!codegen_require_type(ctx->codegen, ctx->structs,
                ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                return_type, &resolved_return)) {
            return 0;
        }

        arg_count = symbol->param_count;
        arg = node->first_child;
        param = symbol->param_list;

        for (index = 0; index < arg_count; index++) {
            TypeDesc arg_type;
            TypeDesc param_type;

            if (!arg || !param) {
                return codegen_set_error(ctx->codegen,
                    "codegen: argument count mismatch");
            }

            if (!codegen_expression_type(ctx, arg, &arg_type)) {
                return 0;
            }

            param_type = codegen_make_type_desc(param->type_token,
                param->pointer_depth, param->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    param_type, &param_type)) {
                return 0;
            }

            if (param_type.pointer_depth > 0) {
                if (arg_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(arg)) {
                    /* ok */
                } else if (!codegen_pointer_compatible(param_type, arg_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: argument type mismatch");
                }
            } else {
                if (param_type.type_token.type == TOKEN_STRUCT) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: struct argument not supported");
                }

                if (!codegen_type_is_integer(arg_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer argument");
                }
            }

            arg = arg->next;
            param = param->next;
        }

        if (arg) {
            return codegen_set_error(ctx->codegen,
                "codegen: argument count mismatch");
        }

        *type_out = resolved_return;
        return 1;
    }

    if (node->type == PARSER_NODE_IDENTIFIER) {
        const LocalSymbol *local = NULL;
        const ParserNode *param = NULL;
        const GlobalSymbol *symbol = NULL;
        TypeDesc desc;
        TypeDesc resolved;

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
            desc = codegen_make_type_desc(local->type_token,
                local->pointer_depth, local->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, desc, &resolved)) {
                return 0;
            }
            *type_out = resolved;
            return 1;
        }

        param = codegen_find_param(ctx, node->token);
        if (param) {
            desc = codegen_make_type_desc(param->type_token,
                param->pointer_depth, param->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, desc, &resolved)) {
                return 0;
            }
            *type_out = resolved;
            return 1;
        }

        symbol = codegen_find_global(ctx, node->token);
        if (!symbol) {
            return codegen_set_error(ctx->codegen,
                "codegen: unknown global");
        }

        desc = codegen_make_type_desc(symbol->type_token,
            symbol->pointer_depth, symbol->is_const);
        if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                ctx->typedef_count, desc, &resolved)) {
            return 0;
        }
        *type_out = resolved;
        return 1;
    }

    if (node->type == PARSER_NODE_MEMBER) {
        return codegen_resolve_member_type(ctx, node, type_out);
    }

    if (node->type == PARSER_NODE_SIZEOF) {
        if (node->first_child) {
            TypeDesc operand_type;

            if (node->first_child->next) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected sizeof operand");
            }

            if (!codegen_expression_type(ctx, node->first_child,
                    &operand_type)) {
                return 0;
            }
            *type_out = codegen_int_type_desc();
            return 1;
        }

        {
            TypeDesc target_type;

            target_type = codegen_make_type_desc(node->type_token,
                node->pointer_depth, node->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    target_type, NULL)) {
                return 0;
            }
        }
        *type_out = codegen_int_type_desc();
        return 1;
    }

    if (node->type == PARSER_NODE_CAST) {
        const ParserNode *operand = node->first_child;
        TypeDesc target_type;
        TypeDesc operand_type;

        if (!operand || operand->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected cast operand");
        }

        target_type = codegen_make_type_desc(node->type_token,
            node->pointer_depth, node->is_const);
        if (!codegen_require_type(ctx->codegen, ctx->structs,
                ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                target_type, &target_type)) {
            return 0;
        }

        if (!codegen_expression_type(ctx, operand, &operand_type)) {
            return 0;
        }

        if (target_type.pointer_depth == 0
            && target_type.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: struct cast not supported");
        }

        if (operand_type.pointer_depth == 0
            && operand_type.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: struct cast not supported");
        }

        *type_out = target_type;
        return 1;
    }

    if (node->type == PARSER_NODE_UNARY) {
        const ParserNode *operand = node->first_child;
        TypeDesc operand_type;

        if (!operand || operand->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected unary operand");
        }

        if (token_is_punct(node->token, "&")) {
            const GlobalSymbol *symbol = NULL;
            TypeDesc base_desc;
            TypeDesc resolved_desc;

            if (operand->type != PARSER_NODE_IDENTIFIER) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected identifier address");
            }

            symbol = codegen_find_global(ctx, operand->token);
            if (!symbol) {
                return codegen_set_error(ctx->codegen,
                    "codegen: unknown global");
            }

            base_desc = codegen_make_type_desc(symbol->type_token,
                symbol->pointer_depth, symbol->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, base_desc, &resolved_desc)) {
                return 0;
            }
            resolved_desc.pointer_depth += 1;
            *type_out = resolved_desc;
            return 1;
        }

        if (!codegen_expression_type(ctx, operand, &operand_type)) {
            return 0;
        }

        if (token_is_punct(node->token, "!")) {
            if (!codegen_type_is_integer(operand_type)
                && operand_type.pointer_depth == 0) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected condition operand");
            }

            *type_out = codegen_int_type_desc();
            return 1;
        }

        if (token_is_punct(node->token, "+")) {
            if (!codegen_type_is_integer(operand_type)) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected integer operand");
            }

            *type_out = operand_type;
            return 1;
        }

        if (token_is_punct(node->token, "-")) {
            if (!codegen_type_is_integer(operand_type)) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected integer operand");
            }

            *type_out = codegen_int_type_desc();
            return 1;
        }

        if (token_is_punct(node->token, "*")) {
            if (operand_type.pointer_depth <= 0) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected pointer operand");
            }

            operand_type.pointer_depth--;
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    operand_type, &operand_type)) {
                return 0;
            }
            *type_out = operand_type;
            return 1;
        }

        return codegen_set_error(ctx->codegen,
            "codegen: expected unary operator");
    }

    if (node->type == PARSER_NODE_BINARY) {
        const ParserNode *left = node->first_child;
        const ParserNode *right = left ? left->next : NULL;
        TypeDesc left_type;
        TypeDesc right_type;

        if (!left || !right || right->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected binary operands");
        }

        if (token_is_punct(node->token, "&&")
            || token_is_punct(node->token, "||")) {
            if (!codegen_expression_type(ctx, left, &left_type)) {
                return 0;
            }
            if (!codegen_expression_type(ctx, right, &right_type)) {
                return 0;
            }
            if (!codegen_type_is_integer(left_type)
                && left_type.pointer_depth == 0) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected condition operand");
            }
            if (!codegen_type_is_integer(right_type)
                && right_type.pointer_depth == 0) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected condition operand");
            }
            *type_out = codegen_int_type_desc();
            return 1;
        }

        if (!codegen_expression_type(ctx, left, &left_type)) {
            return 0;
        }

        if (!codegen_expression_type(ctx, right, &right_type)) {
            return 0;
        }

        if (token_is_punct(node->token, "+")) {
            if (left_type.pointer_depth > 0
                && codegen_type_is_integer(right_type)) {
                *type_out = left_type;
                return 1;
            }
            if (right_type.pointer_depth > 0
                && codegen_type_is_integer(left_type)) {
                *type_out = right_type;
                return 1;
            }
        } else if (token_is_punct(node->token, "-")) {
            if (left_type.pointer_depth > 0
                && codegen_type_is_integer(right_type)) {
                *type_out = left_type;
                return 1;
            }
        }

        if (!codegen_type_is_integer(left_type)
            || !codegen_type_is_integer(right_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer operands");
        }

        *type_out = codegen_int_type_desc();
        return 1;
    }

    return codegen_set_error(ctx->codegen,
        "codegen: expected expression");
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

    if (node->type == PARSER_NODE_SIZEOF) {
        TypeDesc target_type;

        if (node->first_child) {
            if (node->first_child->next) {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected sizeof operand");
            }

            if (!codegen_expression_type(ctx, node->first_child,
                    &target_type)) {
                return 0;
            }
        } else {
            target_type = codegen_make_type_desc(node->type_token,
                node->pointer_depth, node->is_const);
        }

        if (!codegen_emit_sizeof_type(ctx, target_type, value,
                value_size)) {
            return 0;
        }

        *type_out = codegen_int_type_desc();
        return 1;
    }

    if (node->type == PARSER_NODE_CAST) {
        const ParserNode *operand = node->first_child;
        char operand_value[32];
        char cast_value[32];
        char from_type[32];
        char to_type[32];
        TypeDesc target_type;
        TypeDesc operand_type;

        if (!operand || operand->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected cast operand");
        }

        target_type = codegen_make_type_desc(node->type_token,
            node->pointer_depth, node->is_const);
        if (!codegen_require_type(ctx->codegen, ctx->structs,
                ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                target_type, &target_type)) {
            return 0;
        }

        if (!codegen_emit_expression(ctx, operand, operand_value,
                sizeof(operand_value), &operand_type)) {
            return 0;
        }

        if (target_type.pointer_depth == 0
            && target_type.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: struct cast not supported");
        }

        if (operand_type.pointer_depth == 0
            && operand_type.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: struct cast not supported");
        }

        if (target_type.pointer_depth > 0) {
            if (operand_type.pointer_depth > 0) {
                if (!codegen_pointer_compatible(target_type, operand_type)
                    && operand_type.type_token.type != TOKEN_VOID
                    && target_type.type_token.type != TOKEN_VOID) {
                    /* explicit cast allows incompatible pointer types */
                }
                codegen_format_desc_type(operand_type, from_type,
                    sizeof(from_type));
                codegen_format_desc_type(target_type, to_type,
                    sizeof(to_type));
                if (strcmp(from_type, to_type) == 0) {
                    snprintf(value, value_size, "%s", operand_value);
                } else {
                    snprintf(cast_value, sizeof(cast_value), "%%t%d",
                        ctx->next_temp_id++);
                    fprintf(ctx->out, "  %s = bitcast %s %s to %s\n",
                        cast_value,
                        from_type,
                        operand_value,
                        to_type);
                    snprintf(value, value_size, "%s", cast_value);
                }
            } else {
                if (!codegen_type_is_integer(operand_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer cast source");
                }
                codegen_format_desc_type(target_type, to_type, sizeof(to_type));
                snprintf(cast_value, sizeof(cast_value), "%%t%d",
                    ctx->next_temp_id++);
                fprintf(ctx->out, "  %s = inttoptr i32 %s to %s\n",
                    cast_value,
                    operand_value,
                    to_type);
                snprintf(value, value_size, "%s", cast_value);
            }

            *type_out = target_type;
            return 1;
        }

        if (operand_type.pointer_depth > 0) {
            codegen_format_desc_type(operand_type, from_type, sizeof(from_type));
            snprintf(cast_value, sizeof(cast_value), "%%t%d",
                ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = ptrtoint %s %s to i32\n",
                cast_value,
                from_type,
                operand_value);
            snprintf(operand_value, sizeof(operand_value), "%s", cast_value);
            operand_type = codegen_int_type_desc();
        }

        if (!codegen_type_is_integer(operand_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer cast source");
        }

        if (!codegen_emit_integer_cast(ctx, operand_type, target_type,
                operand_value, sizeof(operand_value))) {
            return 0;
        }

        snprintf(value, value_size, "%s", operand_value);
        *type_out = target_type;
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
        TypeDesc return_type;

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
                param->pointer_depth, param->is_const);
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    param_type, &param_type)) {
                free(arg_values);
                free(arg_types);
                return 0;
            }

            if (param_type.pointer_depth > 0) {
                if (arg_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(arg)) {
                    snprintf(arg_values[index], sizeof(arg_values[index]),
                        "null");
                } else if (!codegen_pointer_compatible(param_type, arg_type)) {
                    free(arg_values);
                    free(arg_types);
                    return codegen_set_error(ctx->codegen,
                        "codegen: argument type mismatch");
                }
            } else {
                if (param_type.type_token.type == TOKEN_STRUCT) {
                    free(arg_values);
                    free(arg_types);
                    return codegen_set_error(ctx->codegen,
                        "codegen: struct argument not supported");
                }

                if (!codegen_type_is_integer(arg_type)) {
                    free(arg_values);
                    free(arg_types);
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer argument");
                }
            }

            codegen_format_desc_type(param_type, arg_types[index],
                sizeof(arg_types[index]));

            if (param_type.pointer_depth == 0) {
                if (!codegen_emit_integer_cast(ctx, arg_type, param_type,
                        arg_values[index], sizeof(arg_values[index]))) {
                    free(arg_values);
                    free(arg_types);
                    return 0;
                }
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

        return_type = codegen_make_type_desc(symbol->type_token,
            symbol->pointer_depth, symbol->is_const);
        if (!codegen_require_type(ctx->codegen, ctx->structs,
                ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                return_type, &return_type)) {
            free(arg_values);
            free(arg_types);
            return 0;
        }

        codegen_format_desc_type(return_type, type_name, sizeof(type_name));

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
        *type_out = return_type;
        return 1;
    }

    if (node->type == PARSER_NODE_IDENTIFIER) {
        const LocalSymbol *local = NULL;
        const ParserNode *param = NULL;
        const GlobalSymbol *symbol = NULL;
        char value_type[32];
        char pointer_type[32];
        TypeDesc desc;
        TypeDesc resolved;

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
            desc = codegen_make_type_desc(local->type_token,
                local->pointer_depth, local->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, desc, &resolved)) {
                return 0;
            }

            if (resolved.pointer_depth == 0
                && resolved.type_token.type == TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: struct value not supported");
            }

            codegen_format_desc_type(resolved, value_type, sizeof(value_type));
            codegen_format_type(resolved.type_token,
                resolved.pointer_depth + 1, pointer_type, sizeof(pointer_type));

            snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
            fprintf(ctx->out, "  %s = load %s, %s %s\n",
                value,
                value_type,
                pointer_type,
                local->ir_name);
            *type_out = resolved;
            return 1;
        }

        param = codegen_find_param(ctx, node->token);
        if (param) {
            desc = codegen_make_type_desc(param->type_token,
                param->pointer_depth, param->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, desc, &resolved)) {
                return 0;
            }

            if (resolved.pointer_depth == 0
                && resolved.type_token.type == TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: struct value not supported");
            }

            snprintf(value, value_size, "%%%.*s",
                (int)param->token.length,
                param->token.start);
            *type_out = resolved;
            return 1;
        }

        symbol = codegen_find_global(ctx, node->token);
        if (!symbol) {
            return codegen_set_error(ctx->codegen,
                "codegen: unknown global");
        }

        desc = codegen_make_type_desc(symbol->type_token,
            symbol->pointer_depth, symbol->is_const);
        if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                ctx->typedef_count, desc, &resolved)) {
            return 0;
        }

        if (resolved.pointer_depth == 0
            && resolved.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: struct value not supported");
        }

        codegen_format_desc_type(resolved, value_type, sizeof(value_type));
        codegen_format_type(resolved.type_token,
            resolved.pointer_depth + 1, pointer_type, sizeof(pointer_type));

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = load %s, %s @%.*s\n",
            value,
            value_type,
            pointer_type,
            (int)node->token.length,
            node->token.start);
        *type_out = resolved;
        return 1;
    }

    if (node->type == PARSER_NODE_MEMBER) {
        char member_pointer[32];
        char value_type[32];
        char pointer_type[32];
        TypeDesc field_type;

        if (!codegen_emit_member_pointer(ctx, node, member_pointer,
                sizeof(member_pointer), &field_type)) {
            return 0;
        }

        if (field_type.pointer_depth == 0
            && field_type.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(ctx->codegen,
                "codegen: struct value not supported");
        }

        codegen_format_desc_type(field_type, value_type, sizeof(value_type));
        codegen_format_type(field_type.type_token,
            field_type.pointer_depth + 1, pointer_type, sizeof(pointer_type));

        snprintf(value, value_size, "%%t%d", ctx->next_temp_id++);
        fprintf(ctx->out, "  %s = load %s, %s %s\n",
            value,
            value_type,
            pointer_type,
            member_pointer);
        *type_out = field_type;
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
            TypeDesc base_desc;
            TypeDesc resolved_desc;

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
            base_desc = codegen_make_type_desc(symbol->type_token,
                symbol->pointer_depth, symbol->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, base_desc, &resolved_desc)) {
                return 0;
            }
            resolved_desc.pointer_depth += 1;
            *type_out = resolved_desc;
            return 1;
        }

        if (!codegen_emit_expression(ctx, operand, operand_value,
            sizeof(operand_value), &operand_type)) {
            return 0;
        }

        if (token_is_punct(node->token, "!")) {
            snprintf(temp, sizeof(temp), "%%t%d", ctx->next_temp_id++);
            if (codegen_type_is_integer(operand_type)) {
                fprintf(ctx->out, "  %s = icmp eq i32 %s, 0\n", temp,
                    operand_value);
            } else if (operand_type.pointer_depth > 0) {
                char type_name[32];

                codegen_format_desc_type(operand_type, type_name,
                    sizeof(type_name));
                fprintf(ctx->out, "  %s = icmp eq %s %s, null\n", temp,
                    type_name,
                    operand_value);
            } else {
                return codegen_set_error(ctx->codegen,
                    "codegen: expected condition operand");
            }

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
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    operand_type, &operand_type)) {
                return 0;
            }
            if (operand_type.pointer_depth == 0
                && operand_type.type_token.type == TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: struct value not supported");
            }
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
    TypeDesc declared_type;
    TypeDesc resolved_type;

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

    declared_type = codegen_make_type_desc(node->type_token,
        node->pointer_depth, node->is_const);
    if (!codegen_require_type(ctx->codegen, ctx->structs, ctx->struct_count,
            ctx->typedefs, ctx->typedef_count, declared_type, &resolved_type)) {
        return 0;
    }

    local->type_token = resolved_type.type_token;
    local->pointer_depth = resolved_type.pointer_depth;
    local->is_const = resolved_type.is_const;

    codegen_format_desc_type(resolved_type, type_name, sizeof(type_name));
    fprintf(ctx->out, "  %s = alloca %s\n", local->ir_name, type_name);

    if (!node->first_child) {
        return 1;
    }

    if (node->first_child->next) {
        return codegen_set_error(ctx->codegen,
            "codegen: unexpected initializer list");
    }

    if (resolved_type.pointer_depth == 0
        && resolved_type.type_token.type == TOKEN_STRUCT) {
        return codegen_set_error(ctx->codegen,
            "codegen: struct initializer not supported");
    }

    if (!codegen_emit_expression(ctx, node->first_child, init_value,
        sizeof(init_value), &init_type)) {
        return 0;
    }

    if (resolved_type.pointer_depth > 0) {
        if (init_type.pointer_depth == 0
            && codegen_is_null_pointer_literal(node->first_child)) {
            snprintf(init_value, sizeof(init_value), "null");
        } else if (!codegen_pointer_compatible(resolved_type, init_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: initializer type mismatch");
        }
    } else {
        TypeDesc target_type;

        if (!codegen_type_is_integer(init_type)) {
            return codegen_set_error(ctx->codegen,
                "codegen: expected integer initializer");
        }

        target_type = codegen_make_type_desc(resolved_type.type_token, 0, 0);
        if (!codegen_emit_integer_cast(ctx, init_type, target_type,
                init_value, sizeof(init_value))) {
            return 0;
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

    codegen_push_typedef_scope(ctx);
    for (child = node->first_child; child; child = child->next) {
        if (terminated) {
            break;
        }

        terminated = codegen_emit_statement(ctx, child);
        if (ctx->codegen->error_message) {
            codegen_pop_typedef_scope(ctx);
            return 0;
        }
    }

    codegen_pop_typedef_scope(ctx);
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

    if (!codegen_emit_condition_bool(ctx, condition_type, value, temp,
            sizeof(temp))) {
        return 0;
    }
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

    if (!codegen_emit_condition_bool(ctx, condition_type, value, temp,
            sizeof(temp))) {
        return 0;
    }
    fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
        temp,
        body_label,
        end_label);

    fprintf(ctx->out, "%s:\n", body_label);
    if (!codegen_push_loop(ctx, end_label, cond_label)) {
        return 0;
    }
    body_terminated = codegen_emit_statement(ctx, body);
    codegen_pop_loop(ctx);
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

        if (!codegen_emit_condition_bool(ctx, condition_type, value, temp,
                sizeof(temp))) {
            return 0;
        }
        fprintf(ctx->out, "  br i1 %s, label %%%s, label %%%s\n",
            temp,
            body_label,
            end_label);
    }

    fprintf(ctx->out, "%s:\n", body_label);
    if (!codegen_push_loop(ctx, end_label, inc_label)) {
        return 0;
    }
    body_terminated = codegen_emit_statement(ctx, body);
    codegen_pop_loop(ctx);
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
    TypeDesc expr_type;

    switch (node->type) {
    case PARSER_NODE_BLOCK:
        return codegen_emit_block(ctx, node);
    case PARSER_NODE_DECLARATION:
        if (!codegen_emit_local_declaration(ctx, node)) {
            return 0;
        }
        return 0;
    case PARSER_NODE_TYPEDEF:
        if (!codegen_add_typedef(ctx, node)) {
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
            if (!codegen_require_type(ctx->codegen, ctx->structs,
                    ctx->struct_count, ctx->typedefs, ctx->typedef_count,
                    target_type, &target_type)) {
                return 0;
            }
            if (target_type.is_const) {
                return codegen_set_error(ctx->codegen,
                    "codegen: assignment to const");
            }
            codegen_format_desc_type(target_type, type_name,
                sizeof(type_name));

            if (target_type.pointer_depth > 0) {
                if (expr_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(right)) {
                    snprintf(value, sizeof(value), "null");
                } else if (!codegen_pointer_compatible(target_type,
                        expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: assignment type mismatch");
                }
            } else {
                if (!codegen_type_is_integer(expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer assignment");
                }

                if (!codegen_emit_integer_cast(ctx, expr_type, target_type,
                        value, sizeof(value))) {
                    return 0;
                }
            }

            fprintf(ctx->out, "  store %s %s, %s* %s\n",
                type_name,
                value,
                type_name,
                pointer_value);
            return 0;
        }

        if (left->type == PARSER_NODE_MEMBER) {
            char member_pointer[32];
            TypeDesc target_type;

            if (!codegen_emit_member_pointer(ctx, left, member_pointer,
                    sizeof(member_pointer), &target_type)) {
                return 0;
            }

            if (target_type.pointer_depth == 0
                && target_type.type_token.type == TOKEN_STRUCT) {
                return codegen_set_error(ctx->codegen,
                    "codegen: struct value not supported");
            }

            if (target_type.is_const) {
                return codegen_set_error(ctx->codegen,
                    "codegen: assignment to const");
            }

            if (!codegen_emit_expression(ctx, right, value,
                    sizeof(value), &expr_type)) {
                return 0;
            }

            codegen_format_desc_type(target_type, type_name,
                sizeof(type_name));

            if (target_type.pointer_depth > 0) {
                if (expr_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(right)) {
                    snprintf(value, sizeof(value), "null");
                } else if (!codegen_pointer_compatible(target_type,
                        expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: assignment type mismatch");
                }
            } else {
                if (!codegen_type_is_integer(expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer assignment");
                }

                if (!codegen_emit_integer_cast(ctx, expr_type, target_type,
                        value, sizeof(value))) {
                    return 0;
                }
            }

            fprintf(ctx->out, "  store %s %s, %s* %s\n",
                type_name,
                value,
                type_name,
                member_pointer);
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
            TypeDesc target_type;

            if (local->is_const) {
                return codegen_set_error(ctx->codegen,
                    "codegen: assignment to const");
            }

            target_type = codegen_make_type_desc(local->type_token,
                local->pointer_depth, local->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, target_type, &target_type)) {
                return 0;
            }

            codegen_format_desc_type(target_type, type_name,
                sizeof(type_name));

            if (target_type.pointer_depth > 0) {
                if (expr_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(right)) {
                    snprintf(value, sizeof(value), "null");
                } else if (!codegen_pointer_compatible(target_type,
                        expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: assignment type mismatch");
                }
            } else {
                if (!codegen_type_is_integer(expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer assignment");
                }

                {
                    TypeDesc cast_type;

                    cast_type = codegen_make_type_desc(
                        target_type.type_token, 0, 0);
                    if (!codegen_emit_integer_cast(ctx, expr_type, cast_type,
                            value, sizeof(value))) {
                        return 0;
                    }
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

        {
            TypeDesc target_type;

            if (global->is_const) {
                return codegen_set_error(ctx->codegen,
                    "codegen: assignment to const");
            }

            target_type = codegen_make_type_desc(global->type_token,
                global->pointer_depth, global->is_const);
            if (!codegen_resolve_desc(ctx->codegen, ctx->typedefs,
                    ctx->typedef_count, target_type, &target_type)) {
                return 0;
            }

            codegen_format_desc_type(target_type, type_name,
                sizeof(type_name));

            if (target_type.pointer_depth > 0) {
                if (expr_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(right)) {
                    snprintf(value, sizeof(value), "null");
                } else if (!codegen_pointer_compatible(target_type,
                        expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: assignment type mismatch");
                }
            } else {
                if (!codegen_type_is_integer(expr_type)) {
                    return codegen_set_error(ctx->codegen,
                        "codegen: expected integer assignment");
                }

                {
                    TypeDesc cast_type;

                    cast_type = codegen_make_type_desc(
                        target_type.type_token, 0, 0);
                    if (!codegen_emit_integer_cast(ctx, expr_type, cast_type,
                            value, sizeof(value))) {
                        return 0;
                    }
                }
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
    case PARSER_NODE_BREAK: {
        const LoopContext *loop = codegen_current_loop(ctx);

        if (!loop || node->first_child) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected break statement");
        }

        fprintf(ctx->out, "  br label %%%s\n", loop->break_label);
        return 1;
    }
    case PARSER_NODE_CONTINUE: {
        const LoopContext *loop = codegen_current_loop(ctx);

        if (!loop || node->first_child) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected continue statement");
        }

        fprintf(ctx->out, "  br label %%%s\n", loop->continue_label);
        return 1;
    }
    case PARSER_NODE_RETURN:
        if (!node->first_child || node->first_child->next) {
            return codegen_set_error(ctx->codegen,
                "codegen: unexpected return statement");
        }

        if (!codegen_emit_expression(ctx, node->first_child, value,
            sizeof(value), &expr_type)) {
            return 0;
        }

        {
            TypeDesc return_type;

            return_type = codegen_make_type_desc(ctx->return_type_token,
                ctx->return_pointer_depth, ctx->return_is_const);
            if (return_type.pointer_depth > 0) {
                if (expr_type.pointer_depth == 0
                    && codegen_is_null_pointer_literal(node->first_child)) {
                    snprintf(value, sizeof(value), "null");
                } else if (!codegen_pointer_compatible(return_type,
                        expr_type)) {
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

            {
                TypeDesc cast_type;

                cast_type = codegen_make_type_desc(ctx->return_type_token, 0,
                    0);
                if (!codegen_emit_integer_cast(ctx, expr_type, cast_type,
                        value, sizeof(value))) {
                    return 0;
                }
            }

            fprintf(ctx->out, "  ret %s %s\n", ctx->return_type, value);
            return 1;
        }
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
    const StructSymbol *structs, size_t struct_count,
    const TypedefSymbol *typedefs, size_t typedef_count,
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

    {
        TypeDesc return_desc;
        TypeDesc resolved_return;

        return_desc = codegen_make_type_desc(node->type_token,
            node->pointer_depth, node->is_const);
        if (!codegen_require_type(codegen, structs, struct_count, typedefs,
                typedef_count, return_desc, &resolved_return)) {
            return 0;
        }

        if (resolved_return.pointer_depth == 0
            && resolved_return.type_token.type == TOKEN_STRUCT) {
            return codegen_set_error(codegen,
                "codegen: struct return not supported");
        }

        ctx.return_pointer_depth = resolved_return.pointer_depth;
        ctx.return_type_token = resolved_return.type_token;
        ctx.return_is_const = resolved_return.is_const;
        type_info = codegen_type_info(resolved_return.type_token);
        codegen_format_desc_type(resolved_return, ctx.return_type,
            sizeof(ctx.return_type));
    }

    ctx.codegen = codegen;
    ctx.out = out;
    ctx.next_label_id = 0;
    ctx.next_temp_id = 0;
    ctx.return_width = type_info.width;
    ctx.globals = globals;
    ctx.global_count = global_count;
    ctx.structs = structs;
    ctx.struct_count = struct_count;
    ctx.typedefs = NULL;
    ctx.typedef_count = 0;
    ctx.typedef_capacity = 0;
    ctx.typedef_depth = 0;
    ctx.params = param_list;
    ctx.param_count = param_count;
    ctx.locals = NULL;
    ctx.local_count = 0;
    ctx.local_capacity = 0;
    ctx.functions = functions;
    ctx.function_count = function_count;
    ctx.loop_stack = NULL;
    ctx.loop_depth = 0;
    ctx.loop_capacity = 0;

    if (typedef_count > 0) {
        ctx.typedefs = malloc(typedef_count * sizeof(*ctx.typedefs));
        if (!ctx.typedefs) {
            return codegen_set_error(codegen, "codegen: out of memory");
        }
        memcpy(ctx.typedefs, typedefs, typedef_count * sizeof(*ctx.typedefs));
        ctx.typedef_count = typedef_count;
        ctx.typedef_capacity = typedef_count;
    }

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
            free(ctx.typedefs);
            return codegen_set_error(codegen,
                "codegen: expected parameter");
        }

        {
            TypeDesc param_desc;

            param_desc = codegen_make_type_desc(param->type_token,
                param->pointer_depth, param->is_const);
            if (!codegen_require_type(codegen, structs, struct_count,
                    ctx.typedefs, ctx.typedef_count, param_desc,
                    &param_desc)) {
                free(ctx.typedefs);
                return 0;
            }

            if (param_desc.pointer_depth == 0
                && param_desc.type_token.type == TOKEN_STRUCT) {
                free(ctx.typedefs);
                return codegen_set_error(codegen,
                    "codegen: struct parameter not supported");
            }

            codegen_format_desc_type(param_desc, param_type,
                sizeof(param_type));
        }
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
        free(ctx.typedefs);
        return 0;
    }

    if (!terminated) {
        fprintf(out, "  ret %s 0\n", ctx.return_type);
    }

    fprintf(out, "}\n");
    free(ctx.locals);
    free(ctx.loop_stack);
    free(ctx.typedefs);
    return 1;
}

static int codegen_emit_translation_unit(Codegen *codegen,
    const ParserNode *node,
    FILE *out)
{
    const ParserNode *child = NULL;
    GlobalSymbol *globals = NULL;
    StructSymbol *structs = NULL;
    FunctionSymbol *functions = NULL;
    TypedefSymbol *typedefs = NULL;
    size_t global_count = 0;
    size_t struct_count = 0;
    size_t function_count = 0;
    size_t typedef_count = 0;
    size_t global_index = 0;
    size_t struct_index = 0;
    size_t function_index = 0;
    size_t typedef_index = 0;
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

        if (child->type == PARSER_NODE_TYPEDEF) {
            typedef_count++;
            continue;
        }

        if (child->type == PARSER_NODE_STRUCT) {
            struct_count++;
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

    if (struct_count > 0) {
        structs = malloc(struct_count * sizeof(*structs));
        if (!structs) {
            free(globals);
            return codegen_set_error(codegen,
                "codegen: out of memory");
        }
    }

    if (function_count > 0) {
        functions = malloc(function_count * sizeof(*functions));
        if (!functions) {
            free(globals);
            free(structs);
            return codegen_set_error(codegen,
                "codegen: out of memory");
        }
    }

    if (typedef_count > 0) {
        typedefs = malloc(typedef_count * sizeof(*typedefs));
        if (!typedefs) {
            free(globals);
            free(structs);
            free(functions);
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
            globals[global_index].is_const = child->is_const;
            global_index++;
            continue;
        }

        if (child->type == PARSER_NODE_TYPEDEF) {
            TypeDesc alias_desc;

            alias_desc = codegen_make_type_desc(child->type_token,
                child->pointer_depth, child->is_const);
            if (!codegen_require_type(codegen, structs, struct_count, typedefs,
                    typedef_index, alias_desc, NULL)) {
                goto cleanup;
            }

            typedefs[typedef_index].name = child->token.start;
            typedefs[typedef_index].length = child->token.length;
            typedefs[typedef_index].type = alias_desc;
            typedefs[typedef_index].scope_depth = 0;
            typedef_index++;
            continue;
        }

        if (child->type == PARSER_NODE_STRUCT) {
            size_t field_count = 0;
            const ParserNode *field = NULL;

            if (codegen_find_struct(structs, struct_index, child->token)) {
                codegen_set_error(codegen,
                    "codegen: duplicate struct definition");
                goto cleanup;
            }

            for (field = child->first_child; field; field = field->next) {
                field_count++;
            }

            structs[struct_index].name = child->token.start;
            structs[struct_index].length = child->token.length;
            structs[struct_index].fields = child->first_child;
            structs[struct_index].field_count = field_count;
            struct_index++;
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
            functions[function_index].is_const = child->is_const;
            functions[function_index].param_list = param_list;
            functions[function_index].param_count = param_count;
            function_index++;
            continue;
        }
    }

    for (struct_index = 0; struct_index < struct_count; struct_index++) {
        if (!codegen_emit_struct_definition(codegen, &structs[struct_index],
                structs, struct_count, typedefs, typedef_count, out)) {
            goto cleanup;
        }
    }

    if (struct_count > 0) {
        fprintf(out, "\n");
    }

    for (child = node->first_child; child; child = child->next) {
        if (child->type == PARSER_NODE_DECLARATION) {
            if (!codegen_emit_declaration(codegen, child, globals,
                global_count, structs, struct_count, typedefs, typedef_count,
                out)) {
                goto cleanup;
            }
            continue;
        }

        if (child->type == PARSER_NODE_FUNCTION) {
            if (!codegen_emit_function(codegen, child, globals, global_count,
                structs, struct_count, typedefs, typedef_count, functions,
                function_count, out)) {
                goto cleanup;
            }
            continue;
        }

        if (child->type == PARSER_NODE_STRUCT
            || child->type == PARSER_NODE_TYPEDEF) {
            continue;
        }

        codegen_set_error(codegen, "codegen: unexpected top-level node");
        goto cleanup;
    }

    result = 1;

cleanup:
    free(globals);
    free(structs);
    free(functions);
    free(typedefs);
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
