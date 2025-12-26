# Road to Self-Hosting

This document outlines the features and milestones required for BaseCC to compile its own source code. The items are ordered by their **Difficulty-to-Gain ratio**, starting with the easiest features that provide the most significant progress toward self-hosting.

## Roadmap

### 1. `enum` Support
- **Difficulty**: Low
- **Gain**: High
- **Context**: The entire BaseCC compiler (tokens, AST nodes) relies on `enum`. Currently, these are parsed as identifiers or handled manually. Proper `enum` support (mapping to `int`) would allow the compiler to understand its own core data definitions.

### 2. `switch` and `case` Statements
- **Difficulty**: Medium
- **Gain**: Very High
- **Context**: The parser, checker, and codegen use `switch` statements extensively to dispatch based on node types. Implementing this avoids nested `if-else` chains and drastically improves the compiler's compatibility with its own source.

### 3. Basic Preprocessor (`#include`, `#define`)
- **Difficulty**: Medium
- **Gain**: High
- **Context**: Essential for modularity. While BaseCC uses standard headers, it currently relies on an external preprocessor (like `clang -E`). A minimal internal preprocessor for file inclusion and simple macro substitution is a major step toward independence.

### 4. Type System Expansion (`long`, `unsigned`, `void *`)
- **Difficulty**: Low
- **Gain**: Medium
- **Context**: BaseCC uses `size_t` and general pointers throughout. Adding explicit support for `unsigned` types, `long` (for 64-bit offsets), and `void *` (for generic data structures) ensures type safety when the compiler processes itself.

### 5. Variadic Function Calls
- **Difficulty**: Medium/High
- **Gain**: High (for Debugging/IO)
- **Context**: Necessary for `printf`, `fprintf`, and internal logging/testing. Supporting the `...` syntax in LLVM IR function calls is required for standard C IO compatibility.

### 6. Function Pointers
- **Difficulty**: Medium
- **Gain**: Medium
- **Context**: Mainly used in the test suite (`TestCase` struct) to organize unit tests. Implementing this allows the compiler to run its own test suite.

### 7. Multi-file Compilation & Driver Logic
- **Difficulty**: High
- **Gain**: Essential (Final Milestone)
- **Context**: To self-host, BaseCC must be able to compile multiple `.c` files, generate individual object files (or IR files), and link them together into a single executable.

---

## Current Status
BaseCC currently supports:
- `struct` and `typedef`
- Pointers and Arrays
- Basic control flow (`if`, `while`, `for`)
- LLVM IR generation for arithmetic and logic

> [!TIP]
> Prioritizing **`enum`** and **`switch`** will yield the fastest visible progress, as they are the primary missing pieces in the compiler's front-end logic.
