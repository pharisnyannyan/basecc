#include "checker.h"

#include <string.h>

static int checker_set_error(Checker *checker, const char *message) {
  if (!checker->error_message) {
    checker->error_message = message;
  }

  return 0;
}

void checker_init(Checker *checker, const char *input) {
  parser_init(&checker->parser, input);
  checker->error_message = NULL;
  checker->loop_depth = 0;
}

static int checker_validate_declaration(Checker *checker,
                                        const ParserNode *node);
static int checker_validate_typedef(Checker *checker, const ParserNode *node);

static int checker_validate_number(Checker *checker, const ParserNode *node) {
  if (node->type != PARSER_NODE_NUMBER) {
    return checker_set_error(checker, "checker: expected number");
  }

  if (node->token.type != TOKEN_NUMBER) {
    return checker_set_error(checker, "checker: expected number token");
  }

  if (node->first_child) {
    return checker_set_error(checker, "checker: unexpected number children");
  }

  return 1;
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

static int checker_validate_binary_operator(Checker *checker, Token token) {
  if (token_is_punct(token, "+") || token_is_punct(token, "-") ||
      token_is_punct(token, "*") || token_is_punct(token, "/") ||
      token_is_punct(token, "%") || token_is_punct(token, "&&") ||
      token_is_punct(token, "||")) {
    return 1;
  }

  return checker_set_error(checker, "checker: expected binary operator");
}

static int checker_validate_unary_operator(Checker *checker, Token token) {
  if (token_is_punct(token, "!") || token_is_punct(token, "+") ||
      token_is_punct(token, "-") || token_is_punct(token, "*") ||
      token_is_punct(token, "&")) {
    return 1;
  }

  return checker_set_error(checker, "checker: expected unary operator");
}

static int checker_validate_expression(Checker *checker,
                                       const ParserNode *node) {
  const ParserNode *child = NULL;

  if (node->type == PARSER_NODE_NUMBER) {
    return checker_validate_number(checker, node);
  }

  if (node->type == PARSER_NODE_CALL) {
    if (node->token.type != TOKEN_IDENT) {
      return checker_set_error(checker,
                               "checker: expected function identifier");
    }

    for (child = node->first_child; child; child = child->next) {
      if (!checker_validate_expression(checker, child)) {
        return 0;
      }
    }

    return 1;
  }

  if (node->type == PARSER_NODE_IDENTIFIER) {
    if (node->token.type != TOKEN_IDENT) {
      return checker_set_error(checker, "checker: expected identifier");
    }

    if (node->first_child) {
      return checker_set_error(checker,
                               "checker: unexpected identifier children");
    }

    return 1;
  }

  if (node->type == PARSER_NODE_MEMBER) {
    const ParserNode *base = node->first_child;
    const ParserNode *field = base ? base->next : NULL;

    if (!base || !field || field->next) {
      return checker_set_error(checker, "checker: expected member operands");
    }

    if (!token_is_punct(node->token, ".") &&
        !token_is_punct(node->token, "->")) {
      return checker_set_error(checker, "checker: expected member operator");
    }

    if (field->type != PARSER_NODE_IDENTIFIER ||
        field->token.type != TOKEN_IDENT) {
      return checker_set_error(checker, "checker: expected member identifier");
    }

    return checker_validate_expression(checker, base);
  }

  if (node->type == PARSER_NODE_INDEX) {
    const ParserNode *base = node->first_child;
    const ParserNode *index = base ? base->next : NULL;

    if (!base || !index || index->next) {
      return checker_set_error(checker, "checker: expected index operands");
    }

    if (!token_is_punct(node->token, "[")) {
      return checker_set_error(checker, "checker: expected index operator");
    }

    if (!checker_validate_expression(checker, base)) {
      return 0;
    }

    return checker_validate_expression(checker, index);
  }

  if (node->type == PARSER_NODE_UNARY) {
    const ParserNode *operand = node->first_child;

    if (!operand || operand->next) {
      return checker_set_error(checker, "checker: expected unary operand");
    }

    if (!checker_validate_unary_operator(checker, node->token)) {
      return 0;
    }

    return checker_validate_expression(checker, operand);
  }

  if (node->type == PARSER_NODE_CAST) {
    const ParserNode *operand = node->first_child;

    if (!operand || operand->next) {
      return checker_set_error(checker, "checker: expected cast operand");
    }

    if (node->type_token.type != TOKEN_CHAR &&
        node->type_token.type != TOKEN_SHORT &&
        node->type_token.type != TOKEN_INT &&
        node->type_token.type != TOKEN_VOID &&
        node->type_token.type != TOKEN_STRUCT &&
        node->type_token.type != TOKEN_IDENT) {
      return checker_set_error(checker, "checker: expected cast type");
    }

    return checker_validate_expression(checker, operand);
  }

  if (node->type == PARSER_NODE_SIZEOF) {
    const ParserNode *operand = node->first_child;

    if (operand && operand->next) {
      return checker_set_error(checker, "checker: expected sizeof operand");
    }

    if (!operand) {
      if (node->type_token.type != TOKEN_CHAR &&
          node->type_token.type != TOKEN_SHORT &&
          node->type_token.type != TOKEN_INT &&
          node->type_token.type != TOKEN_VOID &&
          node->type_token.type != TOKEN_STRUCT &&
          node->type_token.type != TOKEN_IDENT) {
        return checker_set_error(checker, "checker: expected sizeof type");
      }

      return 1;
    }

    return checker_validate_expression(checker, operand);
  }

  if (node->type == PARSER_NODE_BINARY) {
    const ParserNode *left = node->first_child;
    const ParserNode *right = left ? left->next : NULL;

    if (!left || !right || right->next) {
      return checker_set_error(checker, "checker: expected binary operands");
    }

    if (!checker_validate_binary_operator(checker, node->token)) {
      return 0;
    }

    if (!checker_validate_expression(checker, left)) {
      return 0;
    }

    return checker_validate_expression(checker, right);
  }

  return checker_set_error(checker, "checker: expected expression");
}

static int checker_validate_assignment(Checker *checker,
                                       const ParserNode *node) {
  const ParserNode *left = node->first_child;
  const ParserNode *right = left ? left->next : NULL;

  if (!left || !right || right->next) {
    return checker_set_error(checker,
                             "checker: expected assignment expression");
  }

  if (left->type == PARSER_NODE_IDENTIFIER) {
    return checker_validate_expression(checker, right);
  }

  if (left->type == PARSER_NODE_UNARY && token_is_punct(left->token, "*")) {
    if (!left->first_child || left->first_child->next) {
      return checker_set_error(checker, "checker: expected assignment target");
    }

    if (!checker_validate_expression(checker, left->first_child)) {
      return 0;
    }

    return checker_validate_expression(checker, right);
  }

  if (left->type == PARSER_NODE_MEMBER) {
    if (!checker_validate_expression(checker, left)) {
      return 0;
    }

    return checker_validate_expression(checker, right);
  }

  if (left->type == PARSER_NODE_INDEX) {
    if (!checker_validate_expression(checker, left)) {
      return 0;
    }

    return checker_validate_expression(checker, right);
  }

  return checker_set_error(checker, "checker: expected assignment target");
}

static int checker_validate_statement(Checker *checker,
                                      const ParserNode *node) {
  const ParserNode *child = NULL;

  switch (node->type) {
  case PARSER_NODE_BLOCK:
    for (child = node->first_child; child; child = child->next) {
      if (!checker_validate_statement(checker, child)) {
        return 0;
      }
    }
    return 1;
  case PARSER_NODE_DECLARATION:
    return checker_validate_declaration(checker, node);
  case PARSER_NODE_TYPEDEF:
    return checker_validate_typedef(checker, node);
  case PARSER_NODE_IF: {
    const ParserNode *condition = node->first_child;
    const ParserNode *then_branch = condition ? condition->next : NULL;
    const ParserNode *else_branch = then_branch ? then_branch->next : NULL;

    if (!condition || !then_branch) {
      return checker_set_error(checker, "checker: incomplete if statement");
    }

    if (else_branch && else_branch->next) {
      return checker_set_error(checker, "checker: unexpected else statement");
    }

    if (!checker_validate_expression(checker, condition)) {
      return 0;
    }

    if (!checker_validate_statement(checker, then_branch)) {
      return 0;
    }

    if (else_branch && !checker_validate_statement(checker, else_branch)) {
      return 0;
    }

    return 1;
  }
  case PARSER_NODE_ASSIGN: {
    return checker_validate_assignment(checker, node);
  }
  case PARSER_NODE_WHILE: {
    const ParserNode *condition = node->first_child;
    const ParserNode *body = condition ? condition->next : NULL;
    int result = 0;

    if (!condition || !body) {
      return checker_set_error(checker, "checker: incomplete while statement");
    }

    if (body->next) {
      return checker_set_error(checker, "checker: unexpected while statement");
    }

    if (!checker_validate_expression(checker, condition)) {
      return 0;
    }

    checker->loop_depth += 1;
    result = checker_validate_statement(checker, body);
    checker->loop_depth -= 1;
    return result;
  }
  case PARSER_NODE_FOR: {
    const ParserNode *init = node->first_child;
    const ParserNode *condition = init ? init->next : NULL;
    const ParserNode *increment = condition ? condition->next : NULL;
    const ParserNode *body = increment ? increment->next : NULL;
    int result = 0;

    if (!init || !condition || !increment || !body) {
      return checker_set_error(checker, "checker: incomplete for statement");
    }

    if (body->next) {
      return checker_set_error(checker, "checker: unexpected for statement");
    }

    if (init->type != PARSER_NODE_EMPTY &&
        init->type != PARSER_NODE_DECLARATION &&
        init->type != PARSER_NODE_ASSIGN) {
      return checker_set_error(checker, "checker: expected for init");
    }

    if (init->type == PARSER_NODE_DECLARATION &&
        !checker_validate_declaration(checker, init)) {
      return 0;
    }

    if (init->type == PARSER_NODE_ASSIGN &&
        !checker_validate_assignment(checker, init)) {
      return 0;
    }

    if (condition->type != PARSER_NODE_EMPTY &&
        !checker_validate_expression(checker, condition)) {
      return 0;
    }

    if (increment->type != PARSER_NODE_EMPTY &&
        increment->type != PARSER_NODE_ASSIGN) {
      return checker_set_error(checker, "checker: expected for increment");
    }

    if (increment->type == PARSER_NODE_ASSIGN &&
        !checker_validate_assignment(checker, increment)) {
      return 0;
    }

    checker->loop_depth += 1;
    result = checker_validate_statement(checker, body);
    checker->loop_depth -= 1;
    return result;
  }
  case PARSER_NODE_RETURN:
    if (!node->first_child || node->first_child->next) {
      return checker_set_error(checker, "checker: unexpected return statement");
    }

    return checker_validate_expression(checker, node->first_child);
  case PARSER_NODE_BREAK:
    if (node->first_child || checker->loop_depth <= 0) {
      return checker_set_error(checker, "checker: unexpected break statement");
    }
    return 1;
  case PARSER_NODE_CONTINUE:
    if (node->first_child || checker->loop_depth <= 0) {
      return checker_set_error(checker,
                               "checker: unexpected continue statement");
    }
    return 1;
  case PARSER_NODE_EMPTY:
    if (node->first_child) {
      return checker_set_error(checker, "checker: unexpected empty statement");
    }
    return 1;
  default:
    return checker_set_error(checker, "checker: expected statement");
  }
}

static int checker_validate_declaration(Checker *checker,
                                        const ParserNode *node) {
  if (node->type != PARSER_NODE_DECLARATION) {
    return checker_set_error(checker, "checker: expected declaration");
  }

  if (node->token.type != TOKEN_IDENT) {
    return checker_set_error(checker, "checker: expected identifier");
  }

  if (node->first_child) {
    if (node->first_child->next) {
      return checker_set_error(checker, "checker: unexpected initializer list");
    }

    if (!checker_validate_expression(checker, node->first_child)) {
      return 0;
    }
  }

  return 1;
}

static int checker_validate_typedef(Checker *checker, const ParserNode *node) {
  if (node->type != PARSER_NODE_TYPEDEF) {
    return checker_set_error(checker, "checker: expected typedef");
  }

  if (node->token.type != TOKEN_IDENT) {
    return checker_set_error(checker, "checker: expected identifier");
  }

  if (node->first_child) {
    return checker_set_error(checker,
                             "checker: unexpected typedef initializer");
  }

  return 1;
}

static int checker_validate_struct_definition(Checker *checker,
                                              const ParserNode *node) {
  const ParserNode *field = NULL;

  if (node->type != PARSER_NODE_STRUCT) {
    return checker_set_error(checker, "checker: expected struct");
  }

  if (node->token.type != TOKEN_IDENT && node->token.type != TOKEN_STRUCT) {
    return checker_set_error(checker, "checker: expected struct name");
  }

  for (field = node->first_child; field; field = field->next) {
    if (field->type != PARSER_NODE_DECLARATION) {
      return checker_set_error(checker, "checker: expected struct field");
    }

    if (field->token.type != TOKEN_IDENT) {
      return checker_set_error(checker,
                               "checker: expected struct field identifier");
    }

    if (field->first_child) {
      return checker_set_error(checker,
                               "checker: unexpected struct field initializer");
    }
  }

  return 1;
}

static int checker_validate_function(Checker *checker, const ParserNode *node) {
  const ParserNode *child = NULL;
  const ParserNode *body = NULL;

  if (node->type != PARSER_NODE_FUNCTION) {
    return checker_set_error(checker, "checker: expected function");
  }

  if (node->token.type != TOKEN_IDENT) {
    return checker_set_error(checker, "checker: expected identifier");
  }

  if (!node->first_child) {
    if (node->is_extern) {
      return 1;
    }
    return checker_set_error(checker, "checker: expected function body");
  }

  for (child = node->first_child; child; child = child->next) {
    if (!child->next && child->type == PARSER_NODE_BLOCK) {
      body = child;
      break;
    }

    if (child->type != PARSER_NODE_DECLARATION) {
      return checker_set_error(checker,
                               "checker: expected parameter declaration");
    }

    if (child->token.type != TOKEN_IDENT) {
      return checker_set_error(checker,
                               "checker: expected parameter identifier");
    }

    if (child->first_child) {
      return checker_set_error(checker,
                               "checker: unexpected parameter initializer");
    }
  }

  if (!body) {
    if (node->is_extern) {
      return 1;
    }
    return checker_set_error(checker, "checker: expected function body");
  }

  if (body->type != PARSER_NODE_BLOCK) {
    return checker_set_error(checker, "checker: expected function block");
  }

  return checker_validate_statement(checker, body);
}

static int checker_validate_enum_definition(Checker *checker,
                                            const ParserNode *node) {
  const ParserNode *enumerator = NULL;

  if (node->type != PARSER_NODE_ENUM) {
    return checker_set_error(checker, "checker: expected enum");
  }

  if (node->token.type != TOKEN_IDENT && node->token.type != TOKEN_ENUM) {
    return checker_set_error(checker, "checker: expected enum name");
  }

  if (!node->first_child) {
    return checker_set_error(checker, "checker: expected enumerator");
  }

  for (enumerator = node->first_child; enumerator;
       enumerator = enumerator->next) {
    if (enumerator->type != PARSER_NODE_ENUMERATOR) {
      return checker_set_error(checker, "checker: expected enumerator");
    }

    if (enumerator->token.type != TOKEN_IDENT) {
      return checker_set_error(checker,
                               "checker: expected enumerator identifier");
    }

    if (enumerator->first_child) {
      if (enumerator->first_child->next) {
        return checker_set_error(
          checker, "checker: unexpected enumerator initializer list");
      }
      if (!checker_validate_expression(checker, enumerator->first_child)) {
        return 0;
      }
    }
  }

  return 1;
}

static int checker_validate_translation_unit(Checker *checker,
                                             const ParserNode *node) {
  const ParserNode *child = NULL;

  if (node->type != PARSER_NODE_TRANSLATION_UNIT) {
    return checker_set_error(checker, "checker: expected translation unit");
  }

  if (node->token.type != TOKEN_EOF) {
    return checker_set_error(checker, "checker: expected EOF token");
  }

  for (child = node->first_child; child; child = child->next) {
    if (child->type == PARSER_NODE_DECLARATION) {
      if (!checker_validate_declaration(checker, child)) {
        return 0;
      }
      continue;
    }

    if (child->type == PARSER_NODE_TYPEDEF) {
      if (!checker_validate_typedef(checker, child)) {
        return 0;
      }
      continue;
    }

    if (child->type == PARSER_NODE_STRUCT) {
      if (!checker_validate_struct_definition(checker, child)) {
        return 0;
      }
      continue;
    }

    if (child->type == PARSER_NODE_ENUM) {
      if (!checker_validate_enum_definition(checker, child)) {
        return 0;
      }
      continue;
    }

    if (child->type == PARSER_NODE_FUNCTION) {
      if (!checker_validate_function(checker, child)) {
        return 0;
      }
      continue;
    }

    return checker_set_error(checker, "checker: unexpected top-level node");
  }

  return 1;
}

int checker_check(Checker *checker) {
  ParserNode *root = parser_parse(&checker->parser);
  const char *parser_message = parser_error(&checker->parser);

  if (!root) {
    return checker_set_error(checker, "checker: out of memory");
  }

  if (parser_message) {
    checker_set_error(checker, parser_message);
    parser_free_node(root);
    return 0;
  }

  if (!checker_validate_translation_unit(checker, root)) {
    parser_free_node(root);
    return 0;
  }

  parser_free_node(root);
  return 1;
}

const char *checker_error(const Checker *checker) {
  return checker->error_message;
}
