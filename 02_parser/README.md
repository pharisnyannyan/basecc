# 02_parser

The parser stage converts a stream of tokens into an Abstract Syntax Tree (AST). It uses a recursive descent parsing strategy to handle the C language grammar.

## Capabilities
- **Recursive Descent**: Handles nesting of expressions, statements, and blocks.
- **AST Generation**: Produces a tree structure representing the program's logical structure.
- **Error Reporting**: Provides descriptive error messages with token location.
- **Typedef Resolution**: The parser maintains a symbol table for typedefs during the parse to disambiguate identifiers from type names.
- **C Grammar Support**: Includes support for most C expressions, control flow statements, struct definitions, and function declarations.
