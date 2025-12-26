# 04_codegen

LLVM IR code generation stage. This stage reuses the lexer, parser, and checker pipelines to generate LLVM IR for the entire translation unit.

## Capabilities
- **Functions**: Generates LLVM functions with parameters and return values.
- **Global Variables**: Supports global scalars, arrays, and structs.
- **Expressions**: Emits IR for arithmetic, logical, and pointer operations.
- **Control Flow**: Implements `if`, `while`, and `for` using LLVM basic blocks and branching.
- **Structs**: Generates LLVM struct types and uses `getelementptr` for member access.
- **Arrays**: Supports indexing and pointer decay.

## Implementation Notes
The codegen uses a `FunctionContext` to track local variables, labels, and temporary IDs. Complex expressions are lowered into a series of LLVM instructions.
