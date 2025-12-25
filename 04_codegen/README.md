# 04_codegen

LLVM IR code generation stage. This stage reuses the lexer, parser, and checker
pipelines to validate declarations, then emits a small LLVM IR module with
`i32` globals for each declaration.
