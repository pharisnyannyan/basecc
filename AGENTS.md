# Agent Coding Guidelines

These guidelines apply to the entire repository.

## Core principles
- Prefer clarity and explicit control flow over cleverness.
- Use simple data structures and readable C.
- Avoid macro-heavy abstractions and surprising tricks.
- Keep changes small and easy to review.
- Treat LLVM IR as the default output, but keep the IR/backend layer swappable for
  alternative IRs, native machine code emission, or custom VM instruction sets.

## Repository structure
- Organize compiler stages as one directory per stage.
- Each stage should include `src/`, `include/` (if needed), `tests/`, and a stage `README.md`.
- Stages should be independently buildable and testable.

## Testing
- Every stage must have unit tests.
- Tests should be small, explicit, and behavior-focused.
- Prefer minimal C tests, shell scripts, or small Python scripts (standard library only).
- Ensure a single top-level command runs the full test suite.
- Run all unit tests after each change and fix any failures before proceeding.
- Run integration tests after each change and fix any failures before proceeding. Use clang for integration tests.

## Build and CI
- Prefer simple Makefiles and incremental build rules.
- Avoid heavy build systems or generators.
- CI must run the same top-level test command on pushes and pull requests.
