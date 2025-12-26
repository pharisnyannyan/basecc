#include "parser.h"

#include <stdlib.h>
#include <string.h>

typedef struct TypedefEntry {
  const char *name;
  size_t length;
  int scope_depth;
} TypedefEntry;

typedef struct TypeSpec {
  Token type_token;
  int pointer_depth;
  int is_const;
  int is_extern;
  int is_static;
} TypeSpec;

void parser_init(Parser *parser, const char *input) {
  lexer_init(&parser->lexer, input);
  parser->error_message = NULL;
  parser->last_token = lexer_next(&parser->lexer);
  parser->typedefs = NULL;
  parser->typedef_count = 0;
  parser->typedef_capacity = 0;
  parser->scope_depth = 0;
}

Token parser_next(Parser *parser) {
  Token current = parser->last_token;

  if (current.type != TOKEN_EOF && current.type != TOKEN_INVALID) {
    parser->last_token = lexer_next(&parser->lexer);
  }

  return current;
}

void parser_node_init(ParserNode *node, ParserNodeType type, Token token) {
  node->type = type;
  node->token = token;
  node->type_token = token;
  node->pointer_depth = 0;
  node->is_const = 0;
  node->is_extern = 0;
  node->is_static = 0;
  node->array_length = 0;
  node->first_child = NULL;
  node->next = NULL;
}

static int token_is_punct(Token token, const char *text) {
  size_t length = strlen(text);

  if (token.type != TOKEN_PUNCT) {
    return 0;
  }

  if (token.length != length) {
    return 0;
  }

  return strncmp(token.start, text, length) == 0;
}

static int token_is_type(Token token) {
  return token.type == TOKEN_INT || token.type == TOKEN_SHORT ||
         token.type == TOKEN_CHAR || token.type == TOKEN_VOID;
}

static int parser_name_matches(Token token, const char *name, size_t length) {
  if (token.length != length) {
    return 0;
  }

  return strncmp(token.start, name, length) == 0;
}

static int parser_is_typedef_name(const Parser *parser, Token token) {
  size_t index = 0;

  if (token.type != TOKEN_IDENT) {
    return 0;
  }

  for (index = parser->typedef_count; index > 0; index--) {
    const TypedefEntry *entry = &parser->typedefs[index - 1];

    if (entry->scope_depth > parser->scope_depth) {
      continue;
    }

    if (parser_name_matches(token, entry->name, entry->length)) {
      return 1;
    }
  }

  return 0;
}

static int parser_push_typedef(Parser *parser, Token name_token) {
  TypedefEntry *entries = NULL;

  if (parser->typedef_count == parser->typedef_capacity) {
    size_t new_capacity =
      parser->typedef_capacity ? parser->typedef_capacity * 2 : 8;

    entries = realloc(parser->typedefs, new_capacity * sizeof(*entries));
    if (!entries) {
      parser->error_message = "parser: out of memory";
      return 0;
    }
    parser->typedefs = entries;
    parser->typedef_capacity = new_capacity;
  }

  entries = parser->typedefs;
  entries[parser->typedef_count].name = name_token.start;
  entries[parser->typedef_count].length = name_token.length;
  entries[parser->typedef_count].scope_depth = parser->scope_depth;
  parser->typedef_count++;
  return 1;
}

static void parser_push_scope(Parser *parser) {
  parser->scope_depth++;
}

static void parser_pop_scope(Parser *parser) {
  while (parser->typedef_count > 0) {
    TypedefEntry *entry = &parser->typedefs[parser->typedef_count - 1];

    if (entry->scope_depth != parser->scope_depth) {
      break;
    }
    parser->typedef_count--;
  }

  if (parser->scope_depth > 0) {
    parser->scope_depth--;
  }
}

static ParserNode *parser_make_error(Parser *parser, Token token,
                                     const char *message);
static Token parser_make_struct_type_token(Token name_token);
static Token parser_make_enum_type_token(Token name_token);

static int parser_parse_type_spec(Parser *parser, TypeSpec *spec,
                                  int allow_storage, ParserNode **error_out) {
  Token token = parser->last_token;
  Token type_token;
  int pointer_depth = 0;
  int is_const = 0;
  int is_extern = 0;
  int is_static = 0;

  for (;;) {
    if (token.type == TOKEN_CONST) {
      is_const = 1;
      parser_next(parser);
      token = parser->last_token;
      continue;
    }

    if (token.type == TOKEN_EXTERN || token.type == TOKEN_STATIC) {
      if (!allow_storage) {
        *error_out = parser_make_error(
          parser, token, "parser: unexpected storage class specifier");
        return 0;
      }
      if (token.type == TOKEN_EXTERN) {
        is_extern = 1;
      } else {
        is_static = 1;
      }
      parser_next(parser);
      token = parser->last_token;
      continue;
    }

    break;
  }

  if (is_extern && is_static) {
    *error_out = parser_make_error(
      parser, token, "parser: conflicting storage class specifiers");
    return 0;
  }

  if (token.type == TOKEN_STRUCT) {
    parser_next(parser);
    token = parser->last_token;
    if (token.type != TOKEN_IDENT) {
      *error_out =
        parser_make_error(parser, token, "parser: expected struct name");
      return 0;
    }
    type_token = parser_make_struct_type_token(token);
    parser_next(parser);
    token = parser->last_token;
  } else if (token.type == TOKEN_ENUM) {
    parser_next(parser);
    token = parser->last_token;
    if (token.type != TOKEN_IDENT) {
      *error_out =
        parser_make_error(parser, token, "parser: expected enum name");
      return 0;
    }
    type_token = parser_make_enum_type_token(token);
    parser_next(parser);
    token = parser->last_token;
  } else if (token_is_type(token) || parser_is_typedef_name(parser, token)) {
    type_token = token;
    parser_next(parser);
    token = parser->last_token;
  } else {
    *error_out = parser_make_error(parser, token, "parser: expected type");
    return 0;
  }

  while (token.type == TOKEN_CONST) {
    is_const = 1;
    parser_next(parser);
    token = parser->last_token;
  }

  while (token_is_punct(token, "*")) {
    pointer_depth++;
    parser_next(parser);
    token = parser->last_token;
  }

  spec->type_token = type_token;
  spec->pointer_depth = pointer_depth;
  spec->is_const = is_const;
  spec->is_extern = is_extern;
  spec->is_static = is_static;
  return 1;
}

static int parser_is_type_start(const Parser *parser, Token token) {
  if (token.type == TOKEN_CONST) {
    return 1;
  }

  if (token.type == TOKEN_STATIC) {
    return 1;
  }

  if (token.type == TOKEN_STRUCT) {
    return 1;
  }

  if (token.type == TOKEN_ENUM) {
    return 1;
  }

  if (token_is_type(token)) {
    return 1;
  }

  return parser_is_typedef_name(parser, token);
}

static Token parser_make_struct_type_token(Token name_token) {
  Token type_token = name_token;

  type_token.type = TOKEN_STRUCT;
  return type_token;
}

static Token parser_make_enum_type_token(Token name_token) {
  Token type_token = name_token;

  type_token.type = TOKEN_ENUM;
  return type_token;
}

static ParserNode *parser_alloc_node(Parser *parser, ParserNodeType type,
                                     Token token) {
  ParserNode *node = malloc(sizeof(*node));

  if (!node) {
    parser->error_message = "parser: out of memory";
    return NULL;
  }

  parser_node_init(node, type, token);
  return node;
}

static ParserNode *parser_make_error(Parser *parser, Token token,
                                     const char *message) {
  ParserNode *node = parser_alloc_node(parser, PARSER_NODE_INVALID, token);

  if (!node) {
    return NULL;
  }

  if (!parser->error_message) {
    parser->error_message = message;
  }

  return node;
}

static int parser_match_punct(Parser *parser, const char *text) {
  Token token = parser->last_token;

  if (!token_is_punct(token, text)) {
    return 0;
  }

  parser_next(parser);
  return 1;
}

static ParserNode *parser_parse_number(Parser *parser) {
  Token token = parser->last_token;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type != TOKEN_NUMBER) {
    return parser_make_error(parser, token, "parser: expected number");
  }

  parser_next(parser);
  return parser_alloc_node(parser, PARSER_NODE_NUMBER, token);
}

static ParserNode *parser_parse_expression(Parser *parser);
static ParserNode *parser_parse_unary(Parser *parser);
static ParserNode *parser_parse_postfix(Parser *parser);
static ParserNode *parser_parse_parameter(Parser *parser);
static ParserNode *parser_parse_assignment_statement(Parser *parser);
static ParserNode *parser_parse_assignment_expression(Parser *parser);
static ParserNode *parser_parse_local_declaration(Parser *parser);
static ParserNode *parser_parse_typedef(Parser *parser);
static ParserNode *parser_parse_struct_definition(Parser *parser,
                                                  Token name_token);
static ParserNode *parser_parse_struct_field(Parser *parser);

typedef struct ParserSnapshot {
  size_t pos;
  Token last_token;
  const char *error_message;
} ParserSnapshot;

static ParserSnapshot parser_snapshot(Parser *parser) {
  ParserSnapshot snapshot;

  snapshot.pos = parser->lexer.pos;
  snapshot.last_token = parser->last_token;
  snapshot.error_message = parser->error_message;
  return snapshot;
}

static void parser_restore(Parser *parser, ParserSnapshot snapshot) {
  parser->lexer.pos = snapshot.pos;
  parser->last_token = snapshot.last_token;
  parser->error_message = snapshot.error_message;
}

static ParserNode *parser_parse_call(Parser *parser, Token name_token) {
  ParserNode *node = NULL;
  ParserNode **tail = NULL;

  if (!parser_match_punct(parser, "(")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '('");
  }

  node = parser_alloc_node(parser, PARSER_NODE_CALL, name_token);
  if (!node) {
    return NULL;
  }

  tail = &node->first_child;

  if (!token_is_punct(parser->last_token, ")")) {
    ParserNode *arg = parser_parse_expression(parser);
    if (!arg || arg->type == PARSER_NODE_INVALID) {
      parser_free_node(node);
      return arg;
    }

    *tail = arg;
    tail = &arg->next;

    while (parser_match_punct(parser, ",")) {
      arg = parser_parse_expression(parser);
      if (!arg || arg->type == PARSER_NODE_INVALID) {
        parser_free_node(node);
        return arg;
      }

      *tail = arg;
      tail = &arg->next;
    }
  }

  if (!parser_match_punct(parser, ")")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ')'");
    parser_free_node(node);
    return error_node;
  }

  return node;
}

static ParserNode *parser_parse_primary(Parser *parser) {
  Token token = parser->last_token;

  if (token.type == TOKEN_NUMBER) {
    return parser_parse_number(parser);
  }

  if (token.type == TOKEN_IDENT) {
    parser_next(parser);
    if (token_is_punct(parser->last_token, "(")) {
      return parser_parse_call(parser, token);
    }

    return parser_alloc_node(parser, PARSER_NODE_IDENTIFIER, token);
  }

  if (token_is_punct(token, "(")) {
    ParserNode *expr = NULL;

    parser_next(parser);
    expr = parser_parse_expression(parser);
    if (!expr || expr->type == PARSER_NODE_INVALID) {
      return expr;
    }

    if (!parser_match_punct(parser, ")")) {
      ParserNode *error_node =
        parser_make_error(parser, parser->last_token, "parser: expected ')'");
      parser_free_node(expr);
      return error_node;
    }

    return expr;
  }

  if (token_is_punct(token, ")")) {
    return parser_make_error(parser, token, "parser: unexpected ')'");
  }

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  return parser_make_error(parser, token, "parser: expected expression");
}

static ParserNode *parser_parse_postfix(Parser *parser) {
  ParserNode *node = parser_parse_primary(parser);

  if (!node || node->type == PARSER_NODE_INVALID) {
    return node;
  }

  while (1) {
    if (token_is_punct(parser->last_token, "[")) {
      Token op_token = parser->last_token;
      ParserNode *index_expr = NULL;
      ParserNode *index = NULL;

      parser_next(parser);
      index_expr = parser_parse_expression(parser);
      if (!index_expr || index_expr->type == PARSER_NODE_INVALID) {
        parser_free_node(node);
        return index_expr;
      }

      if (!parser_match_punct(parser, "]")) {
        ParserNode *error_node =
          parser_make_error(parser, parser->last_token, "parser: expected ']'");
        parser_free_node(node);
        parser_free_node(index_expr);
        return error_node;
      }

      index = parser_alloc_node(parser, PARSER_NODE_INDEX, op_token);
      if (!index) {
        parser_free_node(node);
        parser_free_node(index_expr);
        return NULL;
      }

      index->first_child = node;
      node->next = index_expr;
      node = index;
      continue;
    }

    if (token_is_punct(parser->last_token, ".") ||
        token_is_punct(parser->last_token, "->")) {
      Token op_token = parser->last_token;
      Token field_token;
      ParserNode *field = NULL;
      ParserNode *member = NULL;

      parser_next(parser);
      field_token = parser->last_token;
      if (field_token.type != TOKEN_IDENT) {
        ParserNode *error_node = parser_make_error(
          parser, field_token, "parser: expected field identifier");
        parser_free_node(node);
        return error_node;
      }

      parser_next(parser);
      field = parser_alloc_node(parser, PARSER_NODE_IDENTIFIER, field_token);
      if (!field) {
        parser_free_node(node);
        return NULL;
      }

      member = parser_alloc_node(parser, PARSER_NODE_MEMBER, op_token);
      if (!member) {
        parser_free_node(node);
        parser_free_node(field);
        return NULL;
      }

      member->first_child = node;
      node->next = field;
      node = member;
      continue;
    }

    break;
  }

  return node;
}

static ParserNode *parser_parse_unary(Parser *parser) {
  Token token = parser->last_token;

  if (token.type == TOKEN_SIZEOF) {
    ParserNode *node = NULL;
    ParserNode *operand = NULL;

    parser_next(parser);

    if (parser_match_punct(parser, "(")) {
      TypeSpec spec;
      ParserNode *error_node = NULL;

      if (parser_is_type_start(parser, parser->last_token)) {
        if (!parser_parse_type_spec(parser, &spec, 0, &error_node)) {
          return error_node;
        }

        if (!parser_match_punct(parser, ")")) {
          return parser_make_error(parser, parser->last_token,
                                   "parser: expected ')'");
        }

        node = parser_alloc_node(parser, PARSER_NODE_SIZEOF, token);
        if (!node) {
          return NULL;
        }
        node->type_token = spec.type_token;
        node->pointer_depth = spec.pointer_depth;
        node->is_const = spec.is_const;
        return node;
      }

      operand = parser_parse_expression(parser);
      if (!operand || operand->type == PARSER_NODE_INVALID) {
        return operand;
      }

      if (!parser_match_punct(parser, ")")) {
        ParserNode *error_node =
          parser_make_error(parser, parser->last_token, "parser: expected ')'");
        parser_free_node(operand);
        return error_node;
      }

      node = parser_alloc_node(parser, PARSER_NODE_SIZEOF, token);
      if (!node) {
        parser_free_node(operand);
        return NULL;
      }
      node->first_child = operand;
      return node;
    }

    operand = parser_parse_unary(parser);
    if (!operand || operand->type == PARSER_NODE_INVALID) {
      return operand;
    }

    node = parser_alloc_node(parser, PARSER_NODE_SIZEOF, token);
    if (!node) {
      parser_free_node(operand);
      return NULL;
    }
    node->first_child = operand;
    return node;
  }

  if (token_is_punct(token, "(")) {
    ParserSnapshot snapshot = parser_snapshot(parser);
    ParserNode *error_node = NULL;
    TypeSpec spec;

    parser_next(parser);
    if (parser_is_type_start(parser, parser->last_token)) {
      if (parser_parse_type_spec(parser, &spec, 0, &error_node) &&
          parser_match_punct(parser, ")")) {
        ParserNode *operand = parser_parse_unary(parser);
        ParserNode *node = NULL;

        if (!operand || operand->type == PARSER_NODE_INVALID) {
          return operand;
        }

        node = parser_alloc_node(parser, PARSER_NODE_CAST, token);
        if (!node) {
          parser_free_node(operand);
          return NULL;
        }

        node->type_token = spec.type_token;
        node->pointer_depth = spec.pointer_depth;
        node->is_const = spec.is_const;
        node->first_child = operand;
        return node;
      }
    }

    if (error_node) {
      parser_free_node(error_node);
    }
    parser_restore(parser, snapshot);
  }

  if (token_is_punct(token, "!") || token_is_punct(token, "+") ||
      token_is_punct(token, "-") || token_is_punct(token, "*") ||
      token_is_punct(token, "&")) {
    ParserNode *node = NULL;
    ParserNode *operand = NULL;

    parser_next(parser);
    operand = parser_parse_unary(parser);
    if (!operand || operand->type == PARSER_NODE_INVALID) {
      return operand;
    }

    node = parser_alloc_node(parser, PARSER_NODE_UNARY, token);
    if (!node) {
      parser_free_node(operand);
      return NULL;
    }

    node->first_child = operand;
    return node;
  }

  return parser_parse_postfix(parser);
}

static ParserNode *parser_parse_multiplicative(Parser *parser) {
  ParserNode *left = parser_parse_unary(parser);

  if (!left || left->type == PARSER_NODE_INVALID) {
    return left;
  }

  while (token_is_punct(parser->last_token, "*") ||
         token_is_punct(parser->last_token, "/") ||
         token_is_punct(parser->last_token, "%")) {
    Token op = parser->last_token;
    ParserNode *right = NULL;
    ParserNode *node = NULL;

    parser_next(parser);

    right = parser_parse_unary(parser);
    if (!right || right->type == PARSER_NODE_INVALID) {
      parser_free_node(left);
      return right;
    }

    node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
    if (!node) {
      parser_free_node(left);
      parser_free_node(right);
      return NULL;
    }

    node->first_child = left;
    left->next = right;
    left = node;
  }

  return left;
}

static ParserNode *parser_parse_additive(Parser *parser) {
  ParserNode *left = parser_parse_multiplicative(parser);

  if (!left || left->type == PARSER_NODE_INVALID) {
    return left;
  }

  while (token_is_punct(parser->last_token, "+") ||
         token_is_punct(parser->last_token, "-")) {
    Token op = parser->last_token;
    ParserNode *right = NULL;
    ParserNode *node = NULL;

    parser_next(parser);

    right = parser_parse_multiplicative(parser);
    if (!right || right->type == PARSER_NODE_INVALID) {
      parser_free_node(left);
      return right;
    }

    node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
    if (!node) {
      parser_free_node(left);
      parser_free_node(right);
      return NULL;
    }

    node->first_child = left;
    left->next = right;
    left = node;
  }

  return left;
}

static ParserNode *parser_parse_logical_and(Parser *parser) {
  ParserNode *left = parser_parse_additive(parser);

  if (!left || left->type == PARSER_NODE_INVALID) {
    return left;
  }

  while (token_is_punct(parser->last_token, "&&")) {
    Token op = parser->last_token;
    ParserNode *right = NULL;
    ParserNode *node = NULL;

    parser_next(parser);

    right = parser_parse_additive(parser);
    if (!right || right->type == PARSER_NODE_INVALID) {
      parser_free_node(left);
      return right;
    }

    node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
    if (!node) {
      parser_free_node(left);
      parser_free_node(right);
      return NULL;
    }

    node->first_child = left;
    left->next = right;
    left = node;
  }

  return left;
}

static ParserNode *parser_parse_logical_or(Parser *parser) {
  ParserNode *left = parser_parse_logical_and(parser);

  if (!left || left->type == PARSER_NODE_INVALID) {
    return left;
  }

  while (token_is_punct(parser->last_token, "||")) {
    Token op = parser->last_token;
    ParserNode *right = NULL;
    ParserNode *node = NULL;

    parser_next(parser);

    right = parser_parse_logical_and(parser);
    if (!right || right->type == PARSER_NODE_INVALID) {
      parser_free_node(left);
      return right;
    }

    node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
    if (!node) {
      parser_free_node(left);
      parser_free_node(right);
      return NULL;
    }

    node->first_child = left;
    left->next = right;
    left = node;
  }

  return left;
}

static ParserNode *parser_parse_expression(Parser *parser) {
  return parser_parse_logical_or(parser);
}

static ParserNode *parser_parse_statement(Parser *parser);
static ParserNode *parser_parse_for(Parser *parser);

static ParserNode *parser_parse_block(Parser *parser) {
  Token token = parser->last_token;
  ParserNode *block = NULL;
  ParserNode **tail = NULL;

  if (!parser_match_punct(parser, "{")) {
    return parser_make_error(parser, token, "parser: expected '{'");
  }

  block = parser_alloc_node(parser, PARSER_NODE_BLOCK, token);
  if (!block) {
    return NULL;
  }

  parser_push_scope(parser);
  tail = &block->first_child;

  while (!token_is_punct(parser->last_token, "}")) {
    if (parser->last_token.type == TOKEN_EOF) {
      ParserNode *error_node =
        parser_make_error(parser, parser->last_token, "parser: expected '}'");
      parser_free_node(block);
      parser_pop_scope(parser);
      return error_node;
    }

    if (parser->last_token.type == TOKEN_INVALID) {
      ParserNode *error_node =
        parser_make_error(parser, parser->last_token, "parser: invalid token");
      parser_free_node(block);
      parser_pop_scope(parser);
      return error_node;
    }

    ParserNode *stmt = parser_parse_statement(parser);
    if (!stmt || stmt->type == PARSER_NODE_INVALID) {
      parser_free_node(block);
      parser_pop_scope(parser);
      return stmt;
    }

    *tail = stmt;
    tail = &stmt->next;
  }

  parser_next(parser);
  parser_pop_scope(parser);
  return block;
}

static ParserNode *parser_parse_if(Parser *parser) {
  Token token = parser->last_token;

  parser_next(parser);

  if (!parser_match_punct(parser, "(")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '('");
  }

  ParserNode *condition = parser_parse_expression(parser);
  if (!condition || condition->type == PARSER_NODE_INVALID) {
    return condition;
  }

  if (!parser_match_punct(parser, ")")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ')'");
    parser_free_node(condition);
    return error_node;
  }

  ParserNode *then_branch = parser_parse_statement(parser);
  if (!then_branch || then_branch->type == PARSER_NODE_INVALID) {
    parser_free_node(condition);
    return then_branch;
  }

  ParserNode *node = parser_alloc_node(parser, PARSER_NODE_IF, token);
  if (!node) {
    parser_free_node(condition);
    parser_free_node(then_branch);
    return NULL;
  }

  node->first_child = condition;
  condition->next = then_branch;

  if (parser->last_token.type == TOKEN_ELSE) {
    parser_next(parser);
    ParserNode *else_branch = parser_parse_statement(parser);
    if (!else_branch || else_branch->type == PARSER_NODE_INVALID) {
      parser_free_node(node);
      return else_branch;
    }
    then_branch->next = else_branch;
  }

  return node;
}

static ParserNode *parser_parse_while(Parser *parser) {
  Token token = parser->last_token;

  parser_next(parser);

  if (!parser_match_punct(parser, "(")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '('");
  }

  ParserNode *condition = parser_parse_expression(parser);
  if (!condition || condition->type == PARSER_NODE_INVALID) {
    return condition;
  }

  if (!parser_match_punct(parser, ")")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ')'");
    parser_free_node(condition);
    return error_node;
  }

  ParserNode *body = parser_parse_statement(parser);
  if (!body || body->type == PARSER_NODE_INVALID) {
    parser_free_node(condition);
    return body;
  }

  ParserNode *node = parser_alloc_node(parser, PARSER_NODE_WHILE, token);
  if (!node) {
    parser_free_node(condition);
    parser_free_node(body);
    return NULL;
  }

  node->first_child = condition;
  condition->next = body;
  return node;
}

static ParserNode *parser_parse_for(Parser *parser) {
  Token token = parser->last_token;
  ParserNode *init = NULL;
  ParserNode *condition = NULL;
  ParserNode *increment = NULL;
  ParserNode *body = NULL;
  ParserNode *node = NULL;

  parser_next(parser);

  if (!parser_match_punct(parser, "(")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '('");
  }

  if (parser_match_punct(parser, ";")) {
    init = parser_alloc_node(parser, PARSER_NODE_EMPTY, token);
  } else if (parser_is_type_start(parser, parser->last_token)) {
    init = parser_parse_local_declaration(parser);
  } else if (parser->last_token.type == TOKEN_IDENT ||
             token_is_punct(parser->last_token, "*")) {
    init = parser_parse_assignment_statement(parser);
  } else {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected for init");
  }

  if (!init) {
    return NULL;
  }

  if (init->type == PARSER_NODE_INVALID) {
    return init;
  }

  if (parser_match_punct(parser, ";")) {
    condition = parser_alloc_node(parser, PARSER_NODE_EMPTY, token);
  } else {
    condition = parser_parse_expression(parser);
    if (!condition || condition->type == PARSER_NODE_INVALID) {
      parser_free_node(init);
      return condition;
    }

    if (!parser_match_punct(parser, ";")) {
      ParserNode *error_node =
        parser_make_error(parser, parser->last_token, "parser: expected ';'");
      parser_free_node(init);
      parser_free_node(condition);
      return error_node;
    }
  }

  if (!condition) {
    parser_free_node(init);
    return NULL;
  }

  if (token_is_punct(parser->last_token, ")")) {
    increment = parser_alloc_node(parser, PARSER_NODE_EMPTY, token);
  } else if (parser->last_token.type == TOKEN_IDENT ||
             token_is_punct(parser->last_token, "*")) {
    increment = parser_parse_assignment_expression(parser);
  } else {
    ParserNode *error_node = parser_make_error(
      parser, parser->last_token, "parser: expected for increment");
    parser_free_node(init);
    parser_free_node(condition);
    return error_node;
  }

  if (!increment) {
    parser_free_node(init);
    parser_free_node(condition);
    return NULL;
  }

  if (increment->type == PARSER_NODE_INVALID) {
    parser_free_node(init);
    parser_free_node(condition);
    return increment;
  }

  if (!parser_match_punct(parser, ")")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ')'");
    parser_free_node(init);
    parser_free_node(condition);
    parser_free_node(increment);
    return error_node;
  }

  body = parser_parse_statement(parser);
  if (!body || body->type == PARSER_NODE_INVALID) {
    parser_free_node(init);
    parser_free_node(condition);
    parser_free_node(increment);
    return body;
  }

  node = parser_alloc_node(parser, PARSER_NODE_FOR, token);
  if (!node) {
    parser_free_node(init);
    parser_free_node(condition);
    parser_free_node(increment);
    parser_free_node(body);
    return NULL;
  }

  node->first_child = init;
  init->next = condition;
  condition->next = increment;
  increment->next = body;
  return node;
}

static ParserNode *parser_parse_return(Parser *parser) {
  Token token = parser->last_token;

  parser_next(parser);

  ParserNode *expr = parser_parse_expression(parser);
  if (!expr || expr->type == PARSER_NODE_INVALID) {
    return expr;
  }

  if (!parser_match_punct(parser, ";")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ';'");
    parser_free_node(expr);
    return error_node;
  }

  ParserNode *node = parser_alloc_node(parser, PARSER_NODE_RETURN, token);
  if (!node) {
    parser_free_node(expr);
    return NULL;
  }

  node->first_child = expr;
  return node;
}

static ParserNode *parser_parse_break(Parser *parser) {
  Token token = parser->last_token;

  parser_next(parser);

  if (!parser_match_punct(parser, ";")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected ';'");
  }

  return parser_alloc_node(parser, PARSER_NODE_BREAK, token);
}

static ParserNode *parser_parse_continue(Parser *parser) {
  Token token = parser->last_token;

  parser_next(parser);

  if (!parser_match_punct(parser, ";")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected ';'");
  }

  return parser_alloc_node(parser, PARSER_NODE_CONTINUE, token);
}

static ParserNode *parser_parse_statement(Parser *parser) {
  Token token = parser->last_token;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type == TOKEN_IF) {
    return parser_parse_if(parser);
  }

  if (token.type == TOKEN_WHILE) {
    return parser_parse_while(parser);
  }

  if (token.type == TOKEN_FOR) {
    return parser_parse_for(parser);
  }

  if (token.type == TOKEN_RETURN) {
    return parser_parse_return(parser);
  }

  if (token.type == TOKEN_BREAK) {
    return parser_parse_break(parser);
  }

  if (token.type == TOKEN_CONTINUE) {
    return parser_parse_continue(parser);
  }

  if (token.type == TOKEN_TYPEDEF) {
    return parser_parse_typedef(parser);
  }

  if (parser_is_type_start(parser, token)) {
    return parser_parse_local_declaration(parser);
  }

  if (token_is_punct(token, "{")) {
    return parser_parse_block(parser);
  }

  if (token_is_punct(token, ";")) {
    parser_next(parser);
    return parser_alloc_node(parser, PARSER_NODE_EMPTY, token);
  }

  if (token.type == TOKEN_IDENT || token_is_punct(token, "*")) {
    return parser_parse_assignment_statement(parser);
  }

  return parser_make_error(parser, token, "parser: expected statement");
}

static ParserNode *parser_parse_declaration(Parser *parser, Token name_token,
                                            Token type_token) {
  ParserNode *node =
    parser_alloc_node(parser, PARSER_NODE_DECLARATION, name_token);
  if (!node) {
    return NULL;
  }

  node->type_token = type_token;

  if (parser_match_punct(parser, "[")) {
    Token length_token = parser->last_token;

    if (length_token.type != TOKEN_NUMBER || length_token.value <= 0) {
      ParserNode *error_node =
        parser_make_error(parser, length_token, "parser: expected array size");
      parser_free_node(node);
      return error_node;
    }

    parser_next(parser);

    if (!parser_match_punct(parser, "]")) {
      ParserNode *error_node =
        parser_make_error(parser, parser->last_token, "parser: expected ']'");
      parser_free_node(node);
      return error_node;
    }

    node->array_length = (size_t)length_token.value;
  }

  if (parser_match_punct(parser, "=")) {
    ParserNode *init = parser_parse_expression(parser);

    if (!init || init->type == PARSER_NODE_INVALID) {
      parser_free_node(node);
      return init;
    }

    node->first_child = init;
  }

  if (!parser_match_punct(parser, ";")) {
    Token error_token = parser->last_token;
    ParserNode *error_node =
      parser_make_error(parser, error_token, "parser: expected ';'");
    parser_free_node(node);
    return error_node;
  }

  return node;
}

static ParserNode *parser_parse_typedef(Parser *parser) {
  Token name_token;
  TypeSpec spec;
  ParserNode *node = NULL;
  ParserNode *error_node = NULL;

  parser_next(parser);

  if (!parser_parse_type_spec(parser, &spec, 1, &error_node)) {
    return error_node;
  }

  if (spec.is_extern) {
    return parser_make_error(
      parser, parser->last_token,
      "parser: extern not allowed for local declaration");
  }

  name_token = parser->last_token;
  if (name_token.type != TOKEN_IDENT) {
    return parser_make_error(parser, name_token, "parser: expected identifier");
  }

  parser_next(parser);
  if (parser_match_punct(parser, "=")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: unexpected typedef initializer");
  }

  if (!parser_match_punct(parser, ";")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected ';'");
  }

  node = parser_alloc_node(parser, PARSER_NODE_TYPEDEF, name_token);
  if (!node) {
    return NULL;
  }

  node->type_token = spec.type_token;
  node->pointer_depth = spec.pointer_depth;
  node->is_const = spec.is_const;

  if (!parser_push_typedef(parser, name_token)) {
    parser_free_node(node);
    return NULL;
  }

  return node;
}

static ParserNode *parser_parse_assignment_statement(Parser *parser) {
  ParserNode *node = parser_parse_assignment_expression(parser);

  if (!node || node->type == PARSER_NODE_INVALID) {
    return node;
  }

  if (!parser_match_punct(parser, ";")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ';'");
    parser_free_node(node);
    return error_node;
  }

  return node;
}

static ParserNode *parser_parse_assignment_expression(Parser *parser) {
  ParserNode *left = NULL;
  ParserNode *right = NULL;
  ParserNode *node = NULL;

  left = parser_parse_unary(parser);
  if (!left || left->type == PARSER_NODE_INVALID) {
    return left;
  }

  if (!parser_match_punct(parser, "=")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected '='");
    parser_free_node(left);
    return error_node;
  }

  right = parser_parse_expression(parser);
  if (!right || right->type == PARSER_NODE_INVALID) {
    parser_free_node(left);
    return right;
  }

  node = parser_alloc_node(parser, PARSER_NODE_ASSIGN, left->token);
  if (!node) {
    parser_free_node(left);
    parser_free_node(right);
    return NULL;
  }

  node->first_child = left;
  left->next = right;
  return node;
}

static ParserNode *parser_parse_struct_field(Parser *parser) {
  Token token = parser->last_token;
  TypeSpec spec;
  ParserNode *node = NULL;
  ParserNode *error_node = NULL;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (!parser_parse_type_spec(parser, &spec, 1, &error_node)) {
    return error_node;
  }

  if (spec.is_extern) {
    return parser_make_error(
      parser, parser->last_token,
      "parser: extern not allowed for local declaration");
  }

  token = parser->last_token;
  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type != TOKEN_IDENT) {
    return parser_make_error(parser, token, "parser: expected identifier");
  }

  parser_next(parser);
  if (parser_match_punct(parser, "=")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: unexpected struct field initializer");
  }

  if (!parser_match_punct(parser, ";")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected ';'");
  }

  node = parser_alloc_node(parser, PARSER_NODE_DECLARATION, token);
  if (!node) {
    return NULL;
  }

  node->type_token = spec.type_token;
  node->pointer_depth = spec.pointer_depth;
  node->is_const = spec.is_const;
  return node;
}

static ParserNode *parser_parse_struct_definition(Parser *parser,
                                                  Token name_token) {
  ParserNode *node = NULL;
  ParserNode **tail = NULL;

  if (!parser_match_punct(parser, "{")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '{'");
  }

  node = parser_alloc_node(parser, PARSER_NODE_STRUCT, name_token);
  if (!node) {
    return NULL;
  }

  tail = &node->first_child;

  while (!token_is_punct(parser->last_token, "}")) {
    ParserNode *field = parser_parse_struct_field(parser);

    if (!field || field->type == PARSER_NODE_INVALID) {
      parser_free_node(node);
      return field;
    }

    *tail = field;
    tail = &field->next;
  }

  if (!parser_match_punct(parser, "}")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected '}'");
    parser_free_node(node);
    return error_node;
  }

  if (!parser_match_punct(parser, ";")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ';'");
    parser_free_node(node);
    return error_node;
  }

  return node;
}

static ParserNode *parser_parse_local_declaration(Parser *parser) {
  Token token = parser->last_token;
  TypeSpec spec;
  ParserNode *error_node = NULL;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (!parser_parse_type_spec(parser, &spec, 1, &error_node)) {
    return error_node;
  }

  if (spec.is_extern) {
    return parser_make_error(
      parser, parser->last_token,
      "parser: extern not allowed for local declaration");
  }

  token = parser->last_token;
  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type != TOKEN_IDENT) {
    return parser_make_error(parser, token, "parser: expected identifier");
  }

  parser_next(parser);

  ParserNode *declaration =
    parser_parse_declaration(parser, token, spec.type_token);
  if (declaration && declaration->type != PARSER_NODE_INVALID) {
    declaration->pointer_depth = spec.pointer_depth;
    declaration->is_const = spec.is_const;
    declaration->is_static = spec.is_static;
  }

  return declaration;
}

static ParserNode *parser_parse_parameter(Parser *parser) {
  Token token = parser->last_token;
  TypeSpec spec;
  ParserNode *node = NULL;
  ParserNode *error_node = NULL;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (!parser_parse_type_spec(parser, &spec, 0, &error_node)) {
    return error_node;
  }

  token = parser->last_token;
  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type != TOKEN_IDENT) {
    return parser_make_error(parser, token, "parser: expected identifier");
  }

  parser_next(parser);

  if (parser_match_punct(parser, "[")) {
    Token length_token = parser->last_token;

    if (length_token.type != TOKEN_NUMBER || length_token.value <= 0) {
      return parser_make_error(parser, length_token,
                               "parser: expected array size");
    }

    parser_next(parser);

    if (!parser_match_punct(parser, "]")) {
      return parser_make_error(parser, parser->last_token,
                               "parser: expected ']'");
    }

    spec.pointer_depth += 1;
  }

  node = parser_alloc_node(parser, PARSER_NODE_DECLARATION, token);
  if (!node) {
    return NULL;
  }

  node->type_token = spec.type_token;
  node->pointer_depth = spec.pointer_depth;
  node->is_const = spec.is_const;
  return node;
}

static ParserNode *parser_parse_function(Parser *parser, Token name_token,
                                         Token type_token,
                                         int allow_declaration) {
  ParserNode *params = NULL;
  ParserNode **tail = &params;
  ParserNode *body = NULL;

  if (!parser_match_punct(parser, "(")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '('");
  }

  if (!token_is_punct(parser->last_token, ")")) {
    ParserNode *param = parser_parse_parameter(parser);

    if (!param || param->type == PARSER_NODE_INVALID) {
      return param;
    }

    *tail = param;
    tail = &param->next;

    while (parser_match_punct(parser, ",")) {
      param = parser_parse_parameter(parser);
      if (!param || param->type == PARSER_NODE_INVALID) {
        parser_free_node(params);
        return param;
      }

      *tail = param;
      tail = &param->next;
    }
  }

  if (!parser_match_punct(parser, ")")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ')'");
    parser_free_node(params);
    return error_node;
  }

  if (token_is_punct(parser->last_token, "{")) {
    body = parser_parse_block(parser);
    if (!body || body->type == PARSER_NODE_INVALID) {
      parser_free_node(params);
      return body;
    }
  } else if (allow_declaration && parser_match_punct(parser, ";")) {
    body = NULL;
  } else {
    const char *message = "parser: expected '{'";

    if (allow_declaration) {
      message = "parser: expected '{' or ';'";
    }

    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, message);
    parser_free_node(params);
    return error_node;
  }

  ParserNode *node =
    parser_alloc_node(parser, PARSER_NODE_FUNCTION, name_token);
  if (!node) {
    parser_free_node(params);
    parser_free_node(body);
    return NULL;
  }

  node->type_token = type_token;
  if (params) {
    if (body) {
      *tail = body;
    }
    node->first_child = params;
  } else {
    node->first_child = body;
  }
  return node;
}

static ParserNode *parser_parse_enum_member(Parser *parser) {
  Token token = parser->last_token;
  ParserNode *node = NULL;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type != TOKEN_IDENT) {
    return parser_make_error(parser, token, "parser: expected identifier");
  }

  parser_next(parser);

  node = parser_alloc_node(parser, PARSER_NODE_ENUMERATOR, token);
  if (!node) {
    return NULL;
  }

  if (parser_match_punct(parser, "=")) {
    ParserNode *init = parser_parse_expression(parser);

    if (!init || init->type == PARSER_NODE_INVALID) {
      parser_free_node(node);
      return init;
    }

    node->first_child = init;
  }

  // Comma is handled by the caller (list parser)
  return node;
}

static ParserNode *parser_parse_enum_definition(Parser *parser,
                                                Token name_token) {
  ParserNode *node = NULL;
  ParserNode **tail = NULL;

  if (!parser_match_punct(parser, "{")) {
    return parser_make_error(parser, parser->last_token,
                             "parser: expected '{'");
  }

  node = parser_alloc_node(parser, PARSER_NODE_ENUM, name_token);
  if (!node) {
    return NULL;
  }

  tail = &node->first_child;

  if (!token_is_punct(parser->last_token, "}")) {
    ParserNode *member = parser_parse_enum_member(parser);

    if (!member || member->type == PARSER_NODE_INVALID) {
      parser_free_node(node);
      return member;
    }

    *tail = member;
    tail = &member->next;

    while (parser_match_punct(parser, ",")) {
      if (token_is_punct(parser->last_token, "}")) {
        break; // trailing comma
      }

      member = parser_parse_enum_member(parser);
      if (!member || member->type == PARSER_NODE_INVALID) {
        parser_free_node(node);
        return member;
      }

      *tail = member;
      tail = &member->next;
    }
  }

  if (!parser_match_punct(parser, "}")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected '}'");
    parser_free_node(node);
    return error_node;
  }

  if (!parser_match_punct(parser, ";")) {
    ParserNode *error_node =
      parser_make_error(parser, parser->last_token, "parser: expected ';'");
    parser_free_node(node);
    return error_node;
  }

  return node;
}

static ParserNode *parser_parse_external(Parser *parser) {
  Token token = parser->last_token;
  TypeSpec spec;
  ParserNode *error_node = NULL;

  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type == TOKEN_TYPEDEF) {
    return parser_parse_typedef(parser);
  }

  if (token.type == TOKEN_STRUCT) {
    ParserSnapshot snapshot = parser_snapshot(parser);
    Token name_token;

    parser_next(parser);
    name_token = parser->last_token;
    if (name_token.type != TOKEN_IDENT) {
      return parser_make_error(parser, name_token,
                               "parser: expected struct name");
    }

    parser_next(parser);
    if (token_is_punct(parser->last_token, "{")) {
      return parser_parse_struct_definition(parser, name_token);
    }

    parser_restore(parser, snapshot);
  }

  if (token.type == TOKEN_ENUM) {
    ParserSnapshot snapshot = parser_snapshot(parser);
    Token name_token;

    parser_next(parser);
    name_token = parser->last_token;
    if (name_token.type != TOKEN_IDENT) {
      return parser_make_error(parser, name_token,
                               "parser: expected enum name");
    }

    parser_next(parser);
    if (token_is_punct(parser->last_token, "{")) {
      return parser_parse_enum_definition(parser, name_token);
    }

    parser_restore(parser, snapshot);
  }

  if (!parser_parse_type_spec(parser, &spec, 1, &error_node)) {
    return error_node;
  }

  token = parser->last_token;
  if (token.type == TOKEN_INVALID) {
    return parser_make_error(parser, token, "parser: invalid token");
  }

  if (token.type != TOKEN_IDENT) {
    return parser_make_error(parser, token, "parser: expected identifier");
  }

  parser_next(parser);

  if (token_is_punct(parser->last_token, "(")) {
    ParserNode *function =
      parser_parse_function(parser, token, spec.type_token, spec.is_extern);
    if (function && function->type != PARSER_NODE_INVALID) {
      function->pointer_depth = spec.pointer_depth;
      function->is_const = spec.is_const;
      function->is_extern = spec.is_extern;
      function->is_static = spec.is_static;
    }
    return function;
  }

  ParserNode *declaration =
    parser_parse_declaration(parser, token, spec.type_token);
  if (declaration && declaration->type != PARSER_NODE_INVALID) {
    declaration->pointer_depth = spec.pointer_depth;
    declaration->is_const = spec.is_const;
    declaration->is_extern = spec.is_extern;
    declaration->is_static = spec.is_static;
  }
  return declaration;
}

ParserNode *parser_parse(Parser *parser) {
  ParserNode *root =
    parser_alloc_node(parser, PARSER_NODE_TRANSLATION_UNIT, parser->last_token);
  ParserNode **tail = NULL;

  if (!root) {
    return NULL;
  }

  tail = &root->first_child;

  while (parser->last_token.type != TOKEN_EOF) {
    if (parser->last_token.type == TOKEN_INVALID) {
      ParserNode *error_node =
        parser_make_error(parser, parser->last_token, "parser: invalid token");
      parser_free_node(root);
      return error_node;
    }

    ParserNode *decl = parser_parse_external(parser);

    if (!decl || decl->type == PARSER_NODE_INVALID) {
      parser_free_node(root);
      return decl;
    }

    *tail = decl;
    tail = &decl->next;
  }

  root->token = parser->last_token;
  return root;
}

void parser_free_node(ParserNode *node) {
  if (!node) {
    return;
  }

  parser_free_node(node->first_child);
  parser_free_node(node->next);
  free(node);
}

const char *parser_error(const Parser *parser) {
  return parser->error_message;
}
