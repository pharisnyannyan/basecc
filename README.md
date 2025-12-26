# BaseCC

![CI](https://github.com/pharisnyannyan/basecc/actions/workflows/ci.yml/badge.svg)

BaseCC is a minimal, BSD-style educational C compiler written in C. The primary goal is clarity and learnability. The compiler is designed to be read, understood, and extended step by step, with a long-term intent of becoming self-hosting.

## Current Status

BaseCC has evolved from a minimal lexer/parser into a functional C compiler targeting LLVM IR. It now supports a significant subset of the C language, capable of compiling and running complex algorithms such as Linked Lists, Binary Search Trees, and various Sorting algorithms.

### Supported Features
- **Data Types**: `int`, `char`, pointers, fixed-size arrays.
- **Compound Types**: `struct` (including self-referential pointers) and `typedef`.
- **Control Flow**: `if`/`else`, `while`, `for`, `return`, `break`, `continue`.
- **Operators**: 
  - Arithmetic (`+`, `-`, `*`, `/`)
  - Logical & Comparison (`!`, `==`, `!=`, `<`, `>`, etc.)
  - Member Access (`.`, `->`)
  - Pointer/Memory (`*`, `&`, `sizeof`, `cast`)
- **Storage Classes**: `extern`, `static`, `const`.
- **Functions**: Global function definitions, recursive calls, and external function calls.

## Project Goal

The project deliberately favors:
- **Explicit control flow**: No hidden magic or complex abstractions.
- **Simple data structures**: Easy to reason about memory layout.
- **Boring, readable C**: Follows BSD-leaning coding style.
- **Minimal tooling**: Standard Makefiles and C11.

## Intermediate Representation and Backends

BaseCC targets **LLVM IR as the default intermediate representation**. This keeps the compiler easy to inspect while enabling familiar tooling. The design is modular:
- The IR/backend layer is separated from front-end stages.
- Future goals include direct machine code emission and custom VM backends.

## Repository Structure (Stage-based)

The compiler is organized as **one directory per stage**. Each stage is responsible for a single transformation:

1.  **`01_lexer`**: Lexical analysis, converts source to tokens.
2.  **`02_parser`**: Syntax analysis, builds the Abstract Syntax Tree (AST).
3.  **`03_checker`**: Semantic analysis, performs type checking and name resolution.
4.  **`04_codegen`**: LLVM IR generation.

Each stage directory contains:
- `src/` — Implementation
- `include/` — Headers
- `tests/` — Unit tests for that stage
- `README.md` — Stage-specific documentation

## Build and Testing

Building and testing are designed to be fast and simple.

### Prerequisites
- LLVM/Clang (for code generation and running tests)
- Standard Unix build tools (`make`)

### Commands
- **Build all stages**: `make all`
- **Run all tests**: `make test` (Includes unit tests for each stage and integration tests)
- **Clean**: `make clean`

## Continuous Integration

CI runs the full test suite automatically on every push and pull request via GitHub Actions. All tests must pass, and the code must build without warnings.

## License

BaseCC is released under the MIT License. See `LICENSE` for details.
