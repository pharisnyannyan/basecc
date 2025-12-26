# 03_checker

Semantic analysis stage that builds on the parser AST. This stage performs type checking, name resolution, and structural validation of the C source code.

## Capabilities
- **Type Checking**: Validates expression types, assignment compatibility, and function call arguments.
- **Name Resolution**: Tracks symbols in nested scopes (global, function, block).
- **Struct Validation**: Ensures struct members exist and are accessed correctly.
- **Return Path Analysis**: (Optional/Planned) Validates that functions return values on all paths.
- **Const Validation**: Enforces constant constraints.
